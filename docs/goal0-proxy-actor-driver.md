# Goal 0 proxy actor driver

Iteration 128 defines the first explicit FO4-facing proxy actor driver contract.

Scope is intentionally narrow:

- create remote proxy actor
- move remote proxy actor
- rotate remote proxy actor
- despawn remote proxy actor

It does not attempt animation fidelity, combat state parity, or backend replacement.
Those remain later iterations.

## Why this seam exists

The scene backend stub currently mixes two concerns:

- remote-player replication bookkeeping
- FO4-facing proxy actor presentation behavior

The driver contract isolates the second concern so the remote proxy backend can be shimmed onto a more game-like actor path in Iteration 129 without changing network message shape.

## Contract shape

`fo4_proxy_actor_driver.*` defines:

- `Fo4ProxyActorCreateSpec`
- `Fo4ProxyActorMoveSpec`
- `Fo4ProxyActorRotateSpec`
- `Fo4ProxyActorDriver`

The contract is command-oriented rather than frame-perfect. It gives the future FO4 implementation one stable place to answer:

- what gets created
- what counts as movement application
- what counts as rotation application
- when a proxy is removed

## Validation

The smoke test for this iteration proves:

1. create stores a visible proxy actor record
2. move updates position/velocity/stance state
3. rotate updates presentation rotation independently
4. despawn hides the actor and marks the record removed

That is enough for Iteration 129 to replace scene-backend bookkeeping with a backend-to-driver shim.
