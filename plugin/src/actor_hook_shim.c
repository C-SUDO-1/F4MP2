#include "actor_hook_shim.h"
#include "fo4_bridge_ingress.h"

void ahs_init(void) {}

bool ahs_ingest_actor_spawn(const MsgActorSpawn* msg) { return fbi_ingest_actor_spawn(msg); }
bool ahs_ingest_actor_snapshot(const MsgActorState* msg) { return fbi_ingest_actor_state(msg); }
bool ahs_ingest_actor_despawn(const MsgActorDespawn* msg) { return fbi_ingest_actor_despawn(msg); }
bool ahs_ingest_damage_result(const MsgDamageResult* msg) { return fbi_ingest_damage_result(msg); }
bool ahs_ingest_death_event(const MsgDeathEvent* msg) { return fbi_ingest_death_event(msg); }
