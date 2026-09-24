# Contributing to BDFR_DriveCore

The project is at a source-foundation stage. Please discuss substantial physics changes in an issue before a pull request. Include the Unreal version and the pinned `BDFR_UnifiedPhysicsSystem` commit when reporting a problem.

For a vehicle bug, include reproduction steps, the four wheel bone names and classes, relevant mass/tire/suspension settings, expected behavior and observed behavior. Logs, telemetry and a small test level help isolate problems.

For a pull request:

1. Keep the change scoped to a vehicle feature, integration boundary, documentation fix or dependency update.
2. State which behavior was checked in Unreal and which checks could not be run.
3. Add units and valid ranges for new exposed parameters. Update the README or setup guide when controls or required assets change.
4. Put reusable backend work in `BDFR_UnifiedPhysicsSystem`; put car-specific adapters in this repository. Update the submodule pointer in a separate reviewed change when practical.
5. Avoid committing generated Unreal `Binaries`, `Intermediate`, `Saved` or third-party assets without clear provenance.

There is currently no repository `LICENSE` file. A license and external contribution terms should be established by the owner before inviting code for redistribution.
