# Goal 0 Live Movement Bridge

This slice connects the movement scaffolding to the live UDP service path.

## Added pieces
- `PluginTransportClient`: thin UDP client for HELLO + PLAYER_STATE send/poll.
- `MovementLiveClientBridge`: joins the live service, then relays movement snapshots.
- service-side `MSG_PLAYER_STATE` ingress for the movement-only vertical slice.

## Purpose
This is intentionally narrower than the long-term authority model. It exists to complete Goal 0:

> two players in one session can see each other move.

The service still supports `MSG_INPUT_STATE` for longer-term authority work, but this slice allows movement relay using direct `MSG_PLAYER_STATE` while the real FO4 hook path is being brought up.
