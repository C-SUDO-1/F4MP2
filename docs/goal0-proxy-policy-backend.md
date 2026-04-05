# Goal 0 proxy policy/backend integration

This iteration makes the remote movement policy visible at the scene-backend seam.

## What changed
- `RemoteProxyUpdatePolicy` now has an extended result object.
- `remote_proxy_manager` applies policy with detailed outcome tracking.
- the FO4 scene proxy backend stub records whether the last update was:
  - teleported
  - smoothed in position
  - smoothed in rotation
- the source distance before policy application is also recorded.

## Why it matters
For Goal 0, this makes the remote side less opaque. A future in-game proxy actor backend can use the
same signals to choose between:
- snap/teleport
- smoothed move
- rotation-only update

That gets the movement slice closer to a real visible remote player pipeline without yet requiring
true FO4 actor control.
