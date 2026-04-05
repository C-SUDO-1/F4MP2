# Goal 0 Movement Vertical Slice

Immediate milestone:
- two players join one session
- each sees a remote proxy player
- remote proxy position/rotation/stance update correctly

This iteration adds the first plugin-side movement-specific components:
- local player observer
- remote proxy manager
- movement sync bridge

Current state:
- still synthetic/stub-fed
- no true FO4/CommonLibF4 live hooks yet
- no real in-game actor spawning yet

Purpose:
- narrow the project toward the first true playable co-op slice
- reduce focus on non-movement systems until the two-player movement path works
