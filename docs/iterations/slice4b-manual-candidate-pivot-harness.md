
# Slice 4B — Manual Candidate Pivot Harness

Purpose:
- stop hardcoding the live proof path to `CreatePlayerControlHandlers`
- make the manual candidate and exact addresses selectable at runtime without another source edit

Environment variables:
- `F4MP_MANUAL_CANDIDATE`
  - `controls` (default)
  - `before_input`
  - `after_input`
  - `actor_filtered`
- `F4MP_MANUAL_UPDATE_RVA`
  - RVA for `PlayerCharacter::Update` style manual target
- `F4MP_MANUAL_INPUT_RVA`
  - RVA for input/control-side manual target
- `F4MP_MANUAL_ACTOR_RVA`
  - RVA for `Actor::Update` style manual target

Current behavior:
- default remains the existing `CreatePlayerControlHandlers` path
- alternate candidates only become armable when their required RVAs are supplied

New trace:
- `f4mp_manual_pivot_trace.txt`

Examples:
- default baseline:
  - no env vars set
- before-input pivot:
  - `F4MP_MANUAL_CANDIDATE=before_input`
  - `F4MP_MANUAL_UPDATE_RVA=0x...`
  - `F4MP_MANUAL_INPUT_RVA=0x...`
- after-input pivot:
  - `F4MP_MANUAL_CANDIDATE=after_input`
  - `F4MP_MANUAL_UPDATE_RVA=0x...`
- actor-filtered pivot:
  - `F4MP_MANUAL_CANDIDATE=actor_filtered`
  - `F4MP_MANUAL_ACTOR_RVA=0x...`
