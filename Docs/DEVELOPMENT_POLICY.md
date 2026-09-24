# Development policy

This document states how capabilities are described, integrated and reviewed in BDFR_DriveCore. It is a technical project policy, not a software license.

## Truthful capability status

- **Implemented:** code path exists and is wired into the project; Unreal execution may still be unverified.
- **Verified:** implemented behavior has a reproducible Unreal build and runtime check on named engine version and hardware.
- **Planned:** design intent only. Keep planned features separate from implemented features in README, issues and release notes.
- Numerical defaults are examples until measured vehicle data and calibration results are recorded.

## Physics ownership

- Chaos Vehicles owns the current vehicle chassis and wheel simulation. UnifiedPhysicsSystem supplies the shared backend architecture and is tracked as a dependency.
- Do not imply a Bullet or Hybrid vehicle solver is active because the plugin offers a backend enum or setting. A backend needs a real implementation, vehicle adapter, transform ownership and tests.
- Do not duplicate upstream physics source in DriveCore. Extend upstream APIs in the physics repository; implement vehicle-facing adapters in DriveCore.
- A physics dependency update is a reviewed change: inspect its API and behavior, build DriveCore and record any migration before merging the submodule PR.

## Performance and compatibility claims

- Record Unreal version, platform, scene, vehicle count, physics step and measured frame/physics times for performance claims.
- Add lower-cost quality modes only with documented behavior differences and test them on their target hardware.
- Treat engine upgrades and upstream plugin updates as compatibility work. Passing a static source check does not establish runtime compatibility.

## Changes and releases

- Keep changes focused and document user-visible controls, new parameters, units and defaults.
- For dynamics changes, provide a reproducible scenario with expected and observed behavior; add automated tests where they verify real invariants rather than mirror the code.
- For a release label, require an Unreal build and a basic driving run, with known limitations listed. Until then, describe the project as a source foundation.
- Keep third-party source and asset attribution with the relevant dependency. Do not add unlicensed vehicle meshes, sounds or datasets to the repository.

## Licensing decision

No repository license has been chosen. Before publishing a reuse policy, the owner should add the selected `LICENSE` file and confirm compatibility with every included dependency and asset. This document does not grant additional rights to the code.
