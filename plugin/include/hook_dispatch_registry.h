#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "player_event_translation.h"
#include "actor_event_translation.h"
#include "workshop_event_translation.h"
#include "dialogue_quest_event_translation.h"

typedef struct HookDispatchStats {
    uint32_t player_events;
    uint32_t actor_events;
    uint32_t workshop_events;
    uint32_t dialogue_events;
    uint32_t quest_events;
    uint32_t failures;
} HookDispatchStats;

void hdr_init(void);

bool hdr_dispatch_player_event(const PlayerHostEvent* ev);
bool hdr_dispatch_actor_event(const ActorHostEvent* ev);
bool hdr_dispatch_workshop_event(const WorkshopHostEvent* ev);
bool hdr_dispatch_dialogue_event(const DialogueHostLineEvent* ev);
bool hdr_dispatch_quest_event(const QuestHostObjectiveEvent* ev);

HookDispatchStats hdr_get_stats(void);
