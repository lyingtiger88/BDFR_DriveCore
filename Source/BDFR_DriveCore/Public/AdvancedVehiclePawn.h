#pragma once

#include "CoreMinimal.h"
#include "BDFRPhysicsTypes.h"
#include "WheeledVehiclePawn.h"
#include "AdvancedVehiclePawn.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UChaosWheeledVehicleMovementComponent;

USTRUCT(BlueprintType)
struct FDrivingTelemetry
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category="Driving") float SpeedKmh = 0.f;
    UPROPERTY(BlueprintReadOnly, Category="Driving") float EngineRPM = 0.f;
    UPROPERTY(BlueprintReadOnly, Category="Driving") int32 Gear = 0;
    UPROPERTY(BlueprintReadOnly, Category="Driving") float Throttle = 0.f;
    UPROPERTY(BlueprintReadOnly, Category="Driving") float Brake = 0.f;
    UPROPERTY(BlueprintReadOnly, Category="Driving") float Steering = 0.f;
    UPROPERTY(BlueprintReadOnly, Category="Driving") int32 GroundedWheels = 0;
    UPROPERTY(BlueprintReadOnly, Category="Driving") int32 SlippingWheels = 0;
    UPROPERTY(BlueprintReadOnly, Category="Driving") int32 ABSActiveWheels = 0;
    UPROPERTY(BlueprintReadOnly, Category="Driving") float FuelLiters = 0.f;
    /** Backend selected by UnifiedPhysicsSystem; wheel dynamics still use Chaos Vehicles. */
    UPROPERTY(BlueprintReadOnly, Category="Driving") EBDFRPhysicsBackendType PhysicsBackend = EBDFRPhysicsBackendType::None;
};

/** A Chaos-based car with data-driven driving aids and runtime telemetry. */
UCLASS(Blueprintable)
class BDFR_DRIVECORE_API AAdvancedVehiclePawn : public AWheeledVehiclePawn
{
    GENERATED_BODY()

public:
    AAdvancedVehiclePawn(const FObjectInitializer& ObjectInitializer);

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    UFUNCTION(BlueprintPure, Category="Driving")
    UChaosWheeledVehicleMovementComponent* GetAdvancedMovement() const;

    UFUNCTION(BlueprintPure, Category="Driving")
    FDrivingTelemetry GetTelemetry() const { return Telemetry; }

    /** Scales original wheel friction, e.g. 1=dry, 0.65=wet. Call when surface/weather changes. */
    UFUNCTION(BlueprintCallable, Category="Driving|Environment")
    void SetRoadGripScale(float NewScale);

    UFUNCTION(BlueprintCallable, Category="Driving|Fuel")
    void Refuel(float Liters);

    /** For AI, replay and alternate input devices; call each frame for analog controls. */
    UFUNCTION(BlueprintCallable, Category="Driving|Input")
    void SetDriverInputs(float InThrottle, float InBrake, float InSteering, bool bInHandbrake);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Driving|Camera")
    TObjectPtr<USpringArmComponent> CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Driving|Camera")
    TObjectPtr<UCameraComponent> ChaseCamera;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Driving|Assists", meta=(ClampMin="0", ClampMax="1"))
    float HighSpeedSteeringLimit = 0.45f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Driving|Assists", meta=(ClampMin="1"))
    float SteeringLimitAtKmh = 140.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Driving|Assists", meta=(ClampMin="0"))
    float SteeringResponsePerSecond = 5.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Driving|Fuel", meta=(ClampMin="0"))
    float FuelCapacityLiters = 55.f;

    /** Simplified gameplay fuel estimate; does not feed back into vehicle mass. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Driving|Fuel", meta=(ClampMin="0"))
    float IdleFuelLitersPerHour = 0.8f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Driving|Fuel", meta=(ClampMin="0"))
    float FullThrottleExtraLitersPerHour = 22.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Driving|Fuel")
    bool bConsumeFuel = true;

protected:
    UPROPERTY(BlueprintReadOnly, Category="Driving|Telemetry")
    FDrivingTelemetry Telemetry;

private:
    void InputThrottle(float Value);
    void InputBrake(float Value);
    void InputSteering(float Value);
    void HandbrakePressed();
    void HandbrakeReleased();
    void ShiftUp();
    void ShiftDown();
    void ToggleAutomatic();
    void ResetVehicle();
    void ToggleCamera();
    void ToggleTelemetry();

    float RequestedThrottle = 0.f;
    float RequestedBrake = 0.f;
    float RequestedSteering = 0.f;
    float AppliedSteering = 0.f;
    float RoadGripScale = 1.f;
    bool bHandbrake = false;
    bool bAutomatic = true;
    bool bShowTelemetry = true;
    TArray<float> OriginalWheelFriction;
};
