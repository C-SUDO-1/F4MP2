# Vertical Slice Test Cases

## VS-001 Host start
- Start service
- Expect session creation succeeds

## VS-002 Guest connect
- Send HELLO
- Expect WELCOME

## VS-003 Player state replication
- Guest sends INPUT_STATE for 60 seconds
- Service emits PLAYER_STATE

## VS-004 Bubble enforcement
- Force guest beyond radius
- Expect BUBBLE_VIOLATION

## VS-005 Actor combat
- Spawn one hostile actor
- Guest sends FIRE_INTENT
- Host emits DAMAGE_RESULT

## VS-006 Workshop object
- Guest sends BUILD_REQUEST for whitelisted object
- Expect BUILD_RESULT accepted

## VS-007 Persistence
- Save guest profile
- Disconnect
- Rejoin
- Confirm profile inventory/loadout restored
