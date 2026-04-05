# Phase 1R in-engine Goal 0 validation harness

Iteration 156 adds a single combined validation surface for the next threshold in the stack.

This harness does **not** claim that Fallout 4 / F4SE is live or that a real detour has already been installed.
It does combine the two previously separate validation lines into one Goal 0 readiness check:

- local callback-path validation
- remote proxy-path validation
- toolchain/runtime-profile compatibility check
- remote runtime / scene / driver presence checks

The intent is to answer one narrow question:

**if the current callback candidate and proxy candidate are both behaving coherently, is the stack ready for the first real in-engine Goal 0 movement validation pass?**

A successful harness result means:

- the current runtime profile matches the manifest
- the callback candidate can produce a validated local player sample
- that sample can become a movement state payload
- the payload can drive the remote proxy path coherently
- runtime, scene backend, and proxy driver all agree that a remote player exists

A successful harness result does **not** mean:

- a live FO4 detour is installed
- a real in-world FO4 actor has been spawned
- the first in-engine movement validation has already happened

This iteration exists to make the next threshold falsifiable before the project attempts that real-engine validation.
