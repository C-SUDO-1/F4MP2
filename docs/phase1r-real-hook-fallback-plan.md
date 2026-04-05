# Phase 1R real hook fallback plan

Iteration 159 adds a fallback decision for the first real local hook pass.

Purpose:
- tell the project what to do when the arming gate fails
- keep the next step narrow: hold scaffold, use dry-run resolve-only, or attempt armed validation

Current fallback modes:
- hold_scaffold
- dry_run_resolve_only
- armed_validation

This keeps the hook path crash-aware before a real FO4 detour exists.
