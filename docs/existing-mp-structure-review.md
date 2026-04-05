# Existing FO4 Multiplayer Structure Review

This document records the architectural lessons baked into the scaffold.

## F4MP-style lessons

### What to keep
- explicit client/server split
- authoritative host/service model
- custom NetId space
- real replication protocol

### What to avoid
- trying to sync too much too early
- burying too much logic inside the in-engine layer
- treating unstable archived code as a direct continuation base

## VaultLink-style lessons

### What to keep
- minimal plugin + external networking/service split
- small-scope synchronization first
- proof-first architecture

### What to avoid
- long-term dependence on prototype-only implementation shortcuts

## Tilted / framework lineage lessons

### What to keep
- stronger framework discipline
- shared protocol / serialization structure
- clearer module boundaries

### What to avoid
- assuming a Skyrim-first architecture maps directly to FO4 gameplay/state semantics

## Current scaffold choice
The scaffold intentionally chooses:
- VaultLink-style thin plugin / external service split
- F4MP-style authoritative replication model
- framework discipline inspired by broader multiplayer stacks
- vanilla-first host event mirroring instead of custom gameplay systems
