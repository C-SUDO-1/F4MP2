#include "live_hook_pipeline_stub.h"

void lhps_init(void) { hdr_init(); }
bool lhps_submit_player_event(const PlayerHostEvent* ev) { return hdr_dispatch_player_event(ev); }
bool lhps_submit_actor_event(const ActorHostEvent* ev) { return hdr_dispatch_actor_event(ev); }
bool lhps_submit_workshop_event(const WorkshopHostEvent* ev) { return hdr_dispatch_workshop_event(ev); }
bool lhps_submit_dialogue_event(const DialogueHostLineEvent* ev) { return hdr_dispatch_dialogue_event(ev); }
bool lhps_submit_quest_event(const QuestHostObjectiveEvent* ev) { return hdr_dispatch_quest_event(ev); }
HookDispatchStats lhps_get_stats(void) { return hdr_get_stats(); }
