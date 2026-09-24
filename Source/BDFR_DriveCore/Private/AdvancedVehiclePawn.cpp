#include "AdvancedVehiclePawn.h"
#include "AdvancedVehicleWheel.h"
#include "BDFRPhysicsWorldSubsystem.h"
#include "Camera/CameraComponent.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "ChaosVehicleWheel.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/SpringArmComponent.h"

AAdvancedVehiclePawn::AAdvancedVehiclePawn(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick = true;
    AutoPossessPlayer = EAutoReceiveInput::Player0;

    GetMesh()->SetCollisionProfileName(TEXT("Vehicle"));
    GetMesh()->SetSimulatePhysics(true);

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(GetMesh());
    CameraBoom->TargetArmLength = 550.f;
    CameraBoom->SetRelativeLocation(FVector(-50.f, 0.f, 150.f));
    CameraBoom->bUsePawnControlRotation = false;
    CameraBoom->bEnableCameraLag = true;
    CameraBoom->CameraLagSpeed = 8.f;

    ChaseCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ChaseCamera"));
    ChaseCamera->SetupAttachment(CameraBoom);
    ChaseCamera->bUsePawnControlRotation = false;

    UChaosWheeledVehicleMovementComponent* Movement = GetAdvancedMovement();
    check(Movement);
    Movement->Mass = 1450.f; // kilograms
    Movement->EngineSetup.MaxTorque = 440.f; // Nm
    Movement->EngineSetup.MaxRPM = 6500.f;
    Movement->EngineSetup.EngineIdleRPM = 850.f;
    Movement->EngineSetup.TorqueCurve.GetRichCurve()->Reset();
    Movement->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(0.f, 0.30f);
    Movement->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(1000.f, 0.65f);
    Movement->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(3500.f, 1.00f);
    Movement->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(5500.f, 0.88f);
    Movement->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(6500.f, 0.65f);
    Movement->TransmissionSetup.ForwardGearRatios = {3.60f, 2.19f, 1.52f, 1.19f, 1.00f, 0.82f};
    Movement->TransmissionSetup.ReverseGearRatios = {3.40f};
    Movement->TransmissionSetup.FinalRatio = 3.45f;
    Movement->TransmissionSetup.ChangeUpRPM = 5900.f;
    Movement->TransmissionSetup.ChangeDownRPM = 1800.f;

    // FL, FR, RL, RR. Change bone names to match the imported skeletal mesh.
    Movement->WheelSetups.SetNum(4);
    const FName BoneNames[] = {TEXT("wheel_fl"), TEXT("wheel_fr"), TEXT("wheel_rl"), TEXT("wheel_rr")};
    for (int32 Index = 0; Index < 4; ++Index)
    {
        Movement->WheelSetups[Index].BoneName = BoneNames[Index];
        Movement->WheelSetups[Index].WheelClass = Index < 2 ? UAdvancedFrontWheel::StaticClass() : UAdvancedRearWheel::StaticClass();
    }
}

UChaosWheeledVehicleMovementComponent* AAdvancedVehiclePawn::GetAdvancedMovement() const
{
    return Cast<UChaosWheeledVehicleMovementComponent>(const_cast<AAdvancedVehiclePawn*>(this)->GetVehicleMovement());
}

void AAdvancedVehiclePawn::BeginPlay()
{
    Super::BeginPlay();
    Telemetry.FuelLiters = FuelCapacityLiters;
    if (UWorld* World = GetWorld())
    {
        if (const UBDFRPhysicsWorldSubsystem* Physics = World->GetSubsystem<UBDFRPhysicsWorldSubsystem>())
        {
            Telemetry.PhysicsBackend = Physics->GetActiveBackendType();
            if (Telemetry.PhysicsBackend != EBDFRPhysicsBackendType::Chaos)
            {
                UE_LOG(LogTemp, Warning, TEXT("BDFR_DriveCore: UnifiedPhysicsSystem backend is %d; vehicle wheels remain on Chaos Vehicles until a vehicle backend adapter exists."), static_cast<int32>(Telemetry.PhysicsBackend));
            }
        }
    }
    if (UChaosWheeledVehicleMovementComponent* Movement = GetAdvancedMovement())
    {
        Movement->SetUseAutomaticGears(bAutomatic);
        OriginalWheelFriction.Reset();
        for (const UChaosVehicleWheel* Wheel : Movement->Wheels)
        {
            OriginalWheelFriction.Add(Wheel ? Wheel->FrictionForceMultiplier : 1.f);
        }
        if (Movement->GetNumWheels() == 0)
        {
            UE_LOG(LogTemp, Warning, TEXT("BDFR_DriveCore: configure a skeletal mesh, physics asset and 4 matching wheel bones on %s."), *GetName());
        }
    }
}

void AAdvancedVehiclePawn::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    UChaosWheeledVehicleMovementComponent* Movement = GetAdvancedMovement();
    if (!Movement) return;

    if (UWorld* World = GetWorld())
    {
        if (const UBDFRPhysicsWorldSubsystem* Physics = World->GetSubsystem<UBDFRPhysicsWorldSubsystem>())
            Telemetry.PhysicsBackend = Physics->GetActiveBackendType();
    }

    const float SpeedKmh = GetVelocity().Size() * 0.036f; // cm/s -> km/h
    const float SpeedAlpha = FMath::Clamp(SpeedKmh / FMath::Max(1.f, SteeringLimitAtKmh), 0.f, 1.f);
    const float SteeringLimit = FMath::Lerp(1.f, FMath::Clamp(HighSpeedSteeringLimit, 0.f, 1.f), SpeedAlpha);
    AppliedSteering = FMath::FInterpTo(AppliedSteering, RequestedSteering * SteeringLimit,
        DeltaSeconds, FMath::Max(0.f, SteeringResponsePerSecond));

    float Throttle = FMath::Clamp(RequestedThrottle, 0.f, 1.f);
    if (bConsumeFuel)
    {
        Telemetry.FuelLiters = FMath::Max(0.f, Telemetry.FuelLiters -
            (IdleFuelLitersPerHour + FullThrottleExtraLitersPerHour * Throttle) * DeltaSeconds / 3600.f);
        if (Telemetry.FuelLiters <= 0.f) Throttle = 0.f;
    }
    Movement->SetThrottleInput(Throttle);
    Movement->SetBrakeInput(FMath::Clamp(RequestedBrake, 0.f, 1.f));
    Movement->SetSteeringInput(AppliedSteering);
    Movement->SetHandbrakeInput(bHandbrake);

    Telemetry.SpeedKmh = SpeedKmh;
    Telemetry.EngineRPM = Movement->GetEngineRotationSpeed();
    Telemetry.Gear = Movement->GetCurrentGear();
    Telemetry.Throttle = Throttle;
    Telemetry.Brake = RequestedBrake;
    Telemetry.Steering = AppliedSteering;
    Telemetry.GroundedWheels = 0;
    Telemetry.SlippingWheels = 0;
    Telemetry.ABSActiveWheels = 0;
    for (int32 Index = 0; Index < Movement->GetNumWheels(); ++Index)
    {
        const FWheelStatus& State = Movement->GetWheelState(Index);
        Telemetry.GroundedWheels += State.bInContact ? 1 : 0;
        Telemetry.SlippingWheels += State.bIsSlipping ? 1 : 0;
        Telemetry.ABSActiveWheels += State.bABSActivated ? 1 : 0;
    }

    if (bShowTelemetry && GEngine && IsLocallyControlled())
    {
        GEngine->AddOnScreenDebugMessage(813405, 0.f, FColor::Cyan,
            FString::Printf(TEXT("%3.0f km/h | %4.0f rpm | gear %d | fuel %.1f L | contact %d | slip %d | ABS %d"),
                Telemetry.SpeedKmh, Telemetry.EngineRPM, Telemetry.Gear, Telemetry.FuelLiters,
                Telemetry.GroundedWheels, Telemetry.SlippingWheels, Telemetry.ABSActiveWheels));
    }
}

void AAdvancedVehiclePawn::SetRoadGripScale(float NewScale)
{
    RoadGripScale = FMath::Clamp(NewScale, 0.05f, 3.f);
    if (UChaosWheeledVehicleMovementComponent* Movement = GetAdvancedMovement())
    {
        for (int32 Index = 0; Index < FMath::Min(Movement->GetNumWheels(), OriginalWheelFriction.Num()); ++Index)
        {
            Movement->SetWheelFrictionMultiplier(Index, OriginalWheelFriction[Index] * RoadGripScale);
        }
    }
}

void AAdvancedVehiclePawn::Refuel(float Liters)
{
    Telemetry.FuelLiters = FMath::Clamp(Telemetry.FuelLiters + FMath::Max(0.f, Liters), 0.f, FuelCapacityLiters);
}

void AAdvancedVehiclePawn::SetDriverInputs(float InThrottle, float InBrake, float InSteering, bool bInHandbrake)
{
    RequestedThrottle = FMath::Clamp(InThrottle, 0.f, 1.f);
    RequestedBrake = FMath::Clamp(InBrake, 0.f, 1.f);
    RequestedSteering = FMath::Clamp(InSteering, -1.f, 1.f);
    bHandbrake = bInHandbrake;
}

void AAdvancedVehiclePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    check(PlayerInputComponent);
    PlayerInputComponent->BindAxis(TEXT("DriveThrottle"), this, &AAdvancedVehiclePawn::InputThrottle);
    PlayerInputComponent->BindAxis(TEXT("DriveBrake"), this, &AAdvancedVehiclePawn::InputBrake);
    PlayerInputComponent->BindAxis(TEXT("DriveSteer"), this, &AAdvancedVehiclePawn::InputSteering);
    PlayerInputComponent->BindAction(TEXT("DriveHandbrake"), IE_Pressed, this, &AAdvancedVehiclePawn::HandbrakePressed);
    PlayerInputComponent->BindAction(TEXT("DriveHandbrake"), IE_Released, this, &AAdvancedVehiclePawn::HandbrakeReleased);
    PlayerInputComponent->BindAction(TEXT("DriveShiftUp"), IE_Pressed, this, &AAdvancedVehiclePawn::ShiftUp);
    PlayerInputComponent->BindAction(TEXT("DriveShiftDown"), IE_Pressed, this, &AAdvancedVehiclePawn::ShiftDown);
    PlayerInputComponent->BindAction(TEXT("DriveToggleAutomatic"), IE_Pressed, this, &AAdvancedVehiclePawn::ToggleAutomatic);
    PlayerInputComponent->BindAction(TEXT("DriveReset"), IE_Pressed, this, &AAdvancedVehiclePawn::ResetVehicle);
    PlayerInputComponent->BindAction(TEXT("DriveCamera"), IE_Pressed, this, &AAdvancedVehiclePawn::ToggleCamera);
    PlayerInputComponent->BindAction(TEXT("DriveTelemetry"), IE_Pressed, this, &AAdvancedVehiclePawn::ToggleTelemetry);
}

void AAdvancedVehiclePawn::InputThrottle(float Value) { RequestedThrottle = FMath::Clamp(Value, 0.f, 1.f); }
void AAdvancedVehiclePawn::InputBrake(float Value) { RequestedBrake = FMath::Clamp(Value, 0.f, 1.f); }
void AAdvancedVehiclePawn::InputSteering(float Value) { RequestedSteering = FMath::Clamp(Value, -1.f, 1.f); }
void AAdvancedVehiclePawn::HandbrakePressed() { bHandbrake = true; }
void AAdvancedVehiclePawn::HandbrakeReleased() { bHandbrake = false; }

void AAdvancedVehiclePawn::ShiftUp()
{
    if (!bAutomatic) if (UChaosWheeledVehicleMovementComponent* M = GetAdvancedMovement())
        M->SetTargetGear(FMath::Clamp(M->GetTargetGear() + 1, -1, M->TransmissionSetup.ForwardGearRatios.Num()), false);
}
void AAdvancedVehiclePawn::ShiftDown()
{
    if (!bAutomatic) if (UChaosWheeledVehicleMovementComponent* M = GetAdvancedMovement())
        M->SetTargetGear(FMath::Clamp(M->GetTargetGear() - 1, -1, M->TransmissionSetup.ForwardGearRatios.Num()), false);
}
void AAdvancedVehiclePawn::ToggleAutomatic()
{
    bAutomatic = !bAutomatic;
    if (UChaosWheeledVehicleMovementComponent* M = GetAdvancedMovement()) M->SetUseAutomaticGears(bAutomatic);
}
void AAdvancedVehiclePawn::ResetVehicle()
{
    if (UChaosWheeledVehicleMovementComponent* M = GetAdvancedMovement()) M->ResetVehicleState();
    GetMesh()->SetPhysicsLinearVelocity(FVector::ZeroVector);
    GetMesh()->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
    SetActorLocationAndRotation(GetActorLocation() + FVector(0.f, 0.f, 120.f), FRotator(0.f, GetActorRotation().Yaw, 0.f),
        false, nullptr, ETeleportType::TeleportPhysics);
}
void AAdvancedVehiclePawn::ToggleCamera()
{
    CameraBoom->TargetArmLength = CameraBoom->TargetArmLength > 200.f ? 0.f : 550.f;
    CameraBoom->SetRelativeLocation(CameraBoom->TargetArmLength > 200.f ? FVector(-50.f, 0.f, 150.f) : FVector(110.f, 0.f, 115.f));
}
void AAdvancedVehiclePawn::ToggleTelemetry() { bShowTelemetry = !bShowTelemetry; }
