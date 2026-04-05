# Module Interfaces

## Shared
- Packet writer / reader
- Packet codec
- Message and type definitions
- JSON schemas

## Service
- Session manager
- Session registry
- Authority core
- Authority validation
- Interest manager
- World state store
- Profile store
- Replay log
- Socket transport
- Transport loop

## Plugin
- Proxy runtime
- Protocol apply loop
- Safety policy hooks
- Proxy actor/player contracts

## Boundary rule
- Plugin is engine-facing only
- Service owns multiplayer truth
- Shared protocol remains transport-agnostic and engine-agnostic
