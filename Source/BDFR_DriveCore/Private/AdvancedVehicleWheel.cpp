#include "AdvancedVehicleWheel.h"

UAdvancedFrontWheel::UAdvancedFrontWheel()
{
    WheelRadius = 34.f; // centimeters
    WheelWidth = 22.f;
    WheelMass = 18.f;
    MaxSteerAngle = 35.f;
    MaxBrakeTorque = 2800.f;
    bAffectedBySteering = true;
    bAffectedByEngine = false; // default rear-wheel drive; editable in wheel BP
    bAffectedByBrake = true;
    bAffectedByHandbrake = false;
    bABSEnabled = true;
    bTractionControlEnabled = true;
    SpringRate = 32000.f;
    SuspensionDampingRatio = 0.55f;
    SuspensionMaxRaise = 10.f;
    SuspensionMaxDrop = 12.f;
    RollbarScaling = 0.35f;
    FrictionForceMultiplier = 2.0f;
}

UAdvancedRearWheel::UAdvancedRearWheel()
{
    WheelRadius = 34.f;
    WheelWidth = 24.f;
    WheelMass = 19.f;
    MaxSteerAngle = 0.f;
    MaxBrakeTorque = 2200.f;
    MaxHandBrakeTorque = 4500.f;
    bAffectedBySteering = false;
    bAffectedByEngine = true;
    bAffectedByBrake = true;
    bAffectedByHandbrake = true;
    bABSEnabled = true;
    bTractionControlEnabled = true;
    SpringRate = 35000.f;
    SuspensionDampingRatio = 0.58f;
    SuspensionMaxRaise = 10.f;
    SuspensionMaxDrop = 12.f;
    RollbarScaling = 0.45f;
    FrictionForceMultiplier = 2.0f;
}
