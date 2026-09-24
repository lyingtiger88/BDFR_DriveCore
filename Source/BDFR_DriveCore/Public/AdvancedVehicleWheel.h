#pragma once

#include "CoreMinimal.h"
#include "ChaosVehicleWheel.h"
#include "AdvancedVehicleWheel.generated.h"

/** Edit the defaults in child Wheel Blueprints to tune each axle independently. */
UCLASS(Blueprintable)
class BDFR_DRIVECORE_API UAdvancedFrontWheel : public UChaosVehicleWheel
{
    GENERATED_BODY()
public:
    UAdvancedFrontWheel();
};

UCLASS(Blueprintable)
class BDFR_DRIVECORE_API UAdvancedRearWheel : public UChaosVehicleWheel
{
    GENERATED_BODY()
public:
    UAdvancedRearWheel();
};
