# Slice 3L — Differential Gateway Encoding Test

Purpose:
- keep the stable 15-byte overwrite and real trampoline path
- switch the gateway at runtime without changing the rest of the patch/trampoline layer

Variants:
- default / baseline: `phpr_passthrough_gateway`
- marker variant: set `F4MP_GATEWAY_VARIANT=marker`

Behavior:
- baseline gateway immediately jumps to the realized trampoline
- marker gateway increments the gateway marker counter, then jumps to the trampoline

How to use:
- baseline run:
  - do not set `F4MP_GATEWAY_VARIANT`
- marker run:
  - set `F4MP_GATEWAY_VARIANT=marker` before launching the game

Interpretation:
- baseline survives, marker crashes:
  - the first gateway side effect is the fault line
- both survive:
  - the gateway is not the blocker anymore
- both crash:
  - the remaining fault is below the gateway logic
