# Goal 0 engine-backed stub seams

This iteration adds two stronger Goal 0 seams:

- a CommonLibF4-style local player provider stub that installs through the observer-provider seam
- an FO4-style proxy actor backend stub that installs through the remote-proxy backend seam

Together they let the live movement roundtrip exercise the same shape the real engine path will need:

1. future hook/provider captures local player state
2. movement bridge sends live player packets
3. service relays movement
4. remote proxy backend receives spawn/update calls

These are still stub-backed, but they are closer to the eventual real FO4 integration than the older purely synthetic runtime path.
