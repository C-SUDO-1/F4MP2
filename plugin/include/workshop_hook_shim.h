#pragma once

#include <stdbool.h>
#include "messages_workshop.h"

void whs_init(void);
bool whs_ingest_workshop_spawn(const MsgObjectSpawn* msg);
bool whs_ingest_workshop_event(const MsgObjectUpdate* msg);
bool whs_ingest_workshop_despawn(const MsgObjectDespawn* msg);
