#pragma once

#include <stdbool.h>
#include "messages_actor.h"
#include "messages_combat.h"

void ahs_init(void);
bool ahs_ingest_actor_spawn(const MsgActorSpawn* msg);
bool ahs_ingest_actor_snapshot(const MsgActorState* msg);
bool ahs_ingest_actor_despawn(const MsgActorDespawn* msg);
bool ahs_ingest_damage_result(const MsgDamageResult* msg);
bool ahs_ingest_death_event(const MsgDeathEvent* msg);
