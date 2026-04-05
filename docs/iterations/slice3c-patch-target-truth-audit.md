# Slice 3C - Patch Target Truth Audit

Purpose:
- surface the actual discovery-selected candidate alongside the configured install target
- record the computed patch target symbol/address and the planned shim/bridge destinations
- make the traces say explicitly that no real patch write or trampoline is realized yet

Expected next-run signals:
- discovery-selected candidate and configured attempt target appear in install/live/wiring traces
- patch audit trace shows the effective target address and planned shim/bridge addresses
- if the selected candidate and configured target diverge, the traces show that directly
- if they match but detour entry count stays zero, the researched site is still dead in practice
