#pragma once

#include <stdbool.h>
#include "player_event_translation.h"
#include "actor_event_translation.h"
#include "workshop_event_translation.h"
#include "dialogue_quest_event_translation.h"
#include "hook_dispatch_registry.h"

#ifdef __cplusplus
extern "C" {
#endif

void lhps_init(void);
bool lhps_submit_player_event(const PlayerHostEvent* ev);
bool lhps_submit_actor_event(const ActorHostEvent* ev);
bool lhps_submit_workshop_event(const WorkshopHostEvent* ev);
bool lhps_submit_dialogue_event(const DialogueHostLineEvent* ev);
bool lhps_submit_quest_event(const QuestHostObjectiveEvent* ev);
HookDispatchStats lhps_get_stats(void);

#ifdef __cplusplus
}
#endif
