# Plugin entrypoint shape

This scaffold now includes an optional CommonLibF4/F4SE-style entrypoint stub.

Goals:
- make the DLL boundary explicit
- separate query/load lifecycle from gameplay ingress
- keep the plugin thin and service-oriented

Expected evolution:
1. static entrypoint stub
2. CommonLibF4/F4SE-linked optional target
3. one live hook seam at a time through hook shims and ingress
