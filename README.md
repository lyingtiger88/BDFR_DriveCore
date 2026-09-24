# BDFR_DriveCore

**A configurable Unreal Engine vehicle driving foundation built on Chaos Vehicles.** It is intended to grow alongside [BDFR_UnifiedPhysicsSystem](https://github.com/lyingtiger88/BDFR_UnifiedPhysicsSystem), which is included as a Git submodule.

[راهنمای فارسی / Persian setup guide](README.fa.md) · [Architecture](Docs/ARCHITECTURE.md) · [Development policy](Docs/DEVELOPMENT_POLICY.md) · [Contributing](CONTRIBUTING.md)

> **Current status:** C++ starter project. The source has been checked structurally, but has not been compiled or driven in Unreal Editor. A vehicle Skeletal Mesh, Physics Asset and test level are required before Play. The values in the sample car are starting points, not calibrated measurements of a real vehicle.

## Implemented

| Area | Current behavior |
| --- | --- |
| Vehicle | `AAdvancedVehiclePawn` uses Chaos Vehicles for wheel contact, suspension, powertrain and chassis simulation. |
| Tuning | Sample engine torque curve, six forward ratios, reverse gear, mass, front/rear wheel settings, brakes, suspension and tire grip. Most Chaos settings can be edited in a child Blueprint. |
| Controls | Throttle, brake, steering, handbrake, automatic/manual shifting, vehicle reset and camera toggle. Keyboard mappings are included; `SetDriverInputs` accepts external analog input. |
| Driver aids | Chaos wheel ABS and straight-line traction control flags; speed-sensitive steering limit. |
| Environment | `SetRoadGripScale` changes wheel friction at runtime. |
| Telemetry | Speed, RPM, gear, inputs, ground contact, wheel slip, ABS activity, approximate fuel and selected Unified Physics backend. |
| Shared physics | `BDFR_UnifiedPhysicsSystem` is enabled and linked through its `BDFRPhysicsCore` module. The current vehicle remains a Chaos Vehicles simulation. |

## Get started

1. Install a C++ capable Unreal Engine 5.8 setup. This source update addresses the V7 target-upgrade prompt; compilation and gameplay compatibility have not been verified in the editor.
2. Clone the project with its required plugin:

   ```bash
   git clone --recurse-submodules https://github.com/lyingtiger88/BDFR_DriveCore.git
   cd BDFR_DriveCore
   ```

3. Open `BDFR_DriveCore.uproject` and build the C++ modules. If upgrading an earlier checkout, regenerate project files and rebuild the project and plugin from source.
4. Import a vehicle Skeletal Mesh and create its Physics Asset. Use `+X` forward, `+Z` up, centimeters, and wheel bones `wheel_fl`, `wheel_fr`, `wheel_rl`, `wheel_rr`, or edit the names in `WheelSetups`.
5. Create `BP_AdvancedCar` from `AdvancedVehiclePawn`, assign the mesh and physics asset, confirm the four wheel classes and bone names, then place it above a collidable road in a level. The pawn defaults to Player 0 possession.
6. Press Play. Use **W/S** for throttle/brake, **A/D** to steer, **Space** for handbrake, **G** for transmission mode, **Q/E** to shift manually, **R** to reset, **C** for camera and **T** for telemetry.

See the [Persian guide](README.fa.md) for setup details, parameter locations and troubleshooting. Epic's [Chaos vehicle setup guide](https://dev.epicgames.com/documentation/unreal-engine/how-to-set-up-vehicles-in-unreal-engine) covers the required mesh, physics asset and wheel setup.

## Physics dependency and updates

The plugin is pinned at `Plugins/BDFR_UnifiedPhysicsSystem`. Dependabot checks its `main` branch daily and proposes a submodule update PR. A reviewed merge advances the version used by DriveCore. **A new plugin feature does not automatically alter vehicle behavior:** vehicle integration code must explicitly use its public API. See [the integration contract](Docs/PHYSICS_INTEGRATION.md).

The upstream plugin currently exposes a core backend registry and world subsystem, but no registered vehicle backend. Switching the plugin's backend setting does not convert this Chaos vehicle to Bullet or Hybrid physics. The backend value in telemetry reports the plugin's world selection, while the car itself continues to use Chaos Vehicles.

## Scope and roadmap

This is a gameplay oriented foundation, not a validated full vehicle engineering simulator. Fuel use is an approximate gameplay calculation and does not change chassis mass. Tire and engine temperature, component damage, per-wheel stability control, aquaplaning, network replication, calibrated material response and Bullet vehicle simulation are **not implemented**. Each feature needs a measured model, a documented integration boundary and an Unreal runtime test before being presented as supported.

The [architecture](Docs/ARCHITECTURE.md) describes current ownership and proposed extension points. The [development policy](Docs/DEVELOPMENT_POLICY.md) defines evidence required before marking a capability as complete.

## License status

No `LICENSE` file has been selected for this repository. Choose and add one before advertising reuse or redistribution terms. The upstream physics plugin also states that its license selection is pending.
