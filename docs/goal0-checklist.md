# Goal 0 checklist

Iteration 135 freezes Goal 0 scope around the movement slice.

Checklist:

- service accepts two clients
- both clients receive stable local player IDs
- host movement reaches guest through the live adapter/provider path
- guest creates and updates the remote proxy through the scene-backend/driver path
- explicit disconnect removes the remote proxy
- late join replays the latest movement state
- timeout removes the silent remote proxy
- cadence and presentation defaults are recorded and repeatable
