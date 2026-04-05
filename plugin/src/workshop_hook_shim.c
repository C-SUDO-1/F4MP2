#include "workshop_hook_shim.h"
#include "fo4_bridge_ingress.h"

void whs_init(void) {}

bool whs_ingest_workshop_spawn(const MsgObjectSpawn* msg) { return fbi_ingest_object_spawn(msg); }
bool whs_ingest_workshop_event(const MsgObjectUpdate* msg) { return fbi_ingest_object_update(msg); }
bool whs_ingest_workshop_despawn(const MsgObjectDespawn* msg) { return fbi_ingest_object_despawn(msg); }
