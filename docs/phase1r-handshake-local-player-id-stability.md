# Phase 1R: handshake and local-player-id stability

## Intent
Tighten the live join path before adding more hook sophistication.

This iteration makes the transport/client path resilient to a lost first `HELLO` or a service that starts after the client has already opened.

## What changed
- `PluginTransportClient` now caches the `HELLO` payload and retries it on a short poll cadence until `MSG_WELCOME` is received.
- Pre-welcome packets are counted explicitly.
- Handshake stats are exposed through `ptc_get_handshake_stats(...)`.
- A new transport smoke test proves that a client opened **before** the service starts can still connect and receive a stable local player id once the service comes up.
- A new movement smoke test proves the same behavior through `MovementLiveClientBridge` for two clients.

## Why this matters
The critical wall for Goal 0 is still:

- join
- receive welcome
- latch local player id
- drive movement

If that path is nondeterministic, additional hook-policy realism does not improve prototype credibility.

## What this iteration does not claim
- no real FO4 detour is installed
- no real in-engine actor spawn exists yet
- this is a transport/join stability pass only
