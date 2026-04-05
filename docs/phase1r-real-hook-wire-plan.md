# Phase 1R real hook wire plan

Iteration 160 adds one explicit action surface for the next step.

Purpose:
- convert the fallback state into a single canonical action
- stop reading hook-readiness indirectly from multiple modules

Current actions:
- hold_scaffold
- run_dry_run_only
- wire_real_hook

This iteration still does not wire a real FO4 hook. It only makes the next action explicit.
