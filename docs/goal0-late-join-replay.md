# Goal 0 late-join replay

This slice strengthens the two-player movement milestone by proving that a newly joined client can receive the current remote player state from the service snapshot replay path even if the existing player does not move again after the join.

It also adds an optional forced-resend path in the movement sync bridge so a stationary local player can periodically refresh state when desired.
