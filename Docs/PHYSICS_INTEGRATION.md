# Unified Physics dependency

`BDFR_UnifiedPhysicsSystem` is an upstream plugin and part of the `BDFR_DriveCore` project, tracked as a Git submodule at `Plugins/BDFR_UnifiedPhysicsSystem`. The driving project declares the plugin in `.uproject` and depends on its `BDFRPhysicsCore` C++ module. No plugin source is duplicated in the driving repository.

## Clone and update

```bash
git clone --recurse-submodules https://github.com/lyingtiger88/BDFR_DriveCore.git
cd BDFR_DriveCore
git submodule update --init --recursive
```

To bring new upstream changes into this project, run:

```bash
git submodule update --remote Plugins/BDFR_UnifiedPhysicsSystem
git diff --submodule=log
# Build and run driving regression checks in Unreal.
git add Plugins/BDFR_UnifiedPhysicsSystem
git commit -m "Update UnifiedPhysicsSystem dependency"
```

The submodule pins a reviewed commit. Dependabot checks the `main` branch daily and proposes a pull request to advance the pinned SHA when upstream changes. Review/build/merge that PR to bring new code into DriveCore. Using a new feature in the vehicle may additionally need an adapter or gameplay code. This avoids silently changing physics behavior when the upstream repository changes.

## Current adapter boundary

The project calls `UBDFRPhysicsWorldSubsystem::GetActiveBackendType` and exposes it in `FDrivingTelemetry::PhysicsBackend`. Vehicle wheels, powertrain and suspension still use Chaos Vehicles. The current upstream plugin contains `BDFRPhysicsCore` and the backend registry/router, but it does not yet register a Chaos, Bullet or Hybrid backend or expose a vehicle solver. `None` in telemetry means no backend has been registered. When upstream implements vehicle services, the vehicle adapter belongs in this project and should consume its public interfaces. Switching the project's backend selection alone does not transfer an existing Chaos vehicle to Bullet.

## Archive users

The distributed ZIP includes the initially reviewed plugin source snapshot at `Plugins/BDFR_UnifiedPhysicsSystem`, excluding Git metadata. Open it directly; to receive future upstream commits and Dependabot PRs, use the Git repository with the submodule.
