# Phase 1R: proxy actor live candidate

Iteration 152 makes the first engine-facing proxy actor candidate explicit without claiming a spawned FO4 actor exists yet.

## What it adds

- a named actor class surface: `Actor`
- named engine-facing API candidates:
  - `TESObjectREFR::Create`
  - `TESObjectREFR::SetPosition`
  - `TESObjectREFR::SetRotation`
  - `TESObjectREFR::Disable`
- one wrapper over the spawn contract and proxy driver so the scene backend can use a candidate surface instead of calling the recording driver directly

## What it does not claim

- a real in-world actor reference
- cell attach or navmesh correctness
- live FO4 creation/update/despawn validation
