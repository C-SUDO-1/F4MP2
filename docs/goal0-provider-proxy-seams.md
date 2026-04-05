# Goal 0 provider/proxy seams

This iteration adds two movement-oriented seams that move the project closer to real FO4 integration without pretending the engine hooks already exist.

## Local player observer provider

`local_player_observer` can now be fed by an installed provider callback instead of only a synthetic in-memory snapshot. This is the intended seam for a future CommonLibF4/F4SE-backed local player observation path.

## Remote proxy backend

`remote_proxy_manager` can now target an installed backend instead of always writing directly to the in-memory proxy runtime. This is the intended seam for a future in-game proxy actor driver.

## Goal 0 impact

The movement live client bridge can now run with:
- provider-backed local observation
- backend-driven remote proxy application

That is the correct shape for the first real two-player movement slice.
