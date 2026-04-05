# Phase 1R real hook arming gate

Iteration 158 converts the evidence bundle into an arming decision.

Purpose:
- separate "the harness looks healthy" from "the real hook path may be armed"
- keep hook wiring blocked until evidence, runtime profile, and activation all agree

Current arming gate requires:
- evidence bundle passes
- callback candidate is activated
- supported runtime profile
- candidate path is effectively armed

This is still scaffold logic. It does not install a real detour into FO4.
