# Integrity check notes against known FO4 multiplayer attempts

Keep:
- thin plugin + external service split
- authoritative service/session model
- narrow vertical slices

Avoid:
- trying to synchronize too many FO4 systems before movement visibility works
- burying networking and authority logic directly inside the engine-facing plugin layer
- treating archival code as a drop-in base instead of a reference source
