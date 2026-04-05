# Phase 1R toolchain manifest

Iteration 149 adds a small manifest for the public toolchain assumptions that underpin the current hook prototype path.

Current manifest:
- Fallout 4 runtime profile: `1.10.984`
- Hook runtime profile name: `fo4_1_10_984_player_character_update_before_input`
- F4SE build line: `0.7.2`
- Library layer: `CommonLibF4`
- Address Library required: yes

Purpose:
- keep runtime-profile code aligned with the public toolchain we are actually targeting
- give future hook iterations a single place to check version assumptions before real detour work starts
