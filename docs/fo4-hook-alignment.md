# FO4 Hook Alignment (vanilla-first)

This document aligns the scaffold to realistic Fallout 4 / F4SE plugin ingress points without claiming a finished hook map.

## Design rule
The plugin should mirror vanilla FO4 systems and export them into the protocol layer.
It should not invent new game systems.

## Primary ingress categories

### 1. Player state
Used for:
- local transform
- stance / anim state
- equipped weapon
- local use / fire intents

Maps to protocol:
- INPUT_STATE
- PLAYER_STATE
- FIRE_INTENT
- MELEE_INTENT

### 2. Actor lifecycle
Used for:
- hostile spawn / despawn
- hostile state changes
- death state

Maps to protocol:
- ACTOR_SPAWN
- ACTOR_STATE
- ACTOR_DESPAWN
- DAMAGE_RESULT
- DEATH_EVENT

### 3. Workshop interactions
Used for:
- place / move / scrap requests
- local workshop mode transitions

Maps to protocol:
- BUILD_REQUEST
- OBJECT_MOVE_REQ
- OBJECT_SCRAP_REQ

### 4. Dialogue
Used for:
- dialogue start
- current line
- available choices
- dialogue end
- guest choice selection requests

Maps to protocol:
- DIALOGUE_OPEN
- DIALOGUE_LINE
- DIALOGUE_CHOICES
- DIALOGUE_SELECT
- DIALOGUE_CLOSE

### 5. Quest/objective mirror
Used for:
- tracked quest updates
- objective list changes
- stage changes that should be mirrored as UI state

Maps to protocol:
- QUEST_UPDATE

## Hooking priorities

### Priority A
- player state extraction
- actor state extraction
- workshop requests

### Priority B
- dialogue mirror ingress
- quest mirror ingress

### Priority C
- richer inventory/profile synchronization
- finer combat / LOS validation support

## Important non-goals
- do not try to run guest quest logic locally
- do not let guest clients own world mutation
- do not distribute AI authority
- do not treat custom wave gameplay as a protocol primitive

## Practical implication
The current engine bridge stub should evolve into a narrow FO4-facing adapter layer:
- capture vanilla events
- convert them to protocol-friendly structs
- never make network decisions itself
