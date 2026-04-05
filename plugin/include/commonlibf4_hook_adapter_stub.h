#pragma once

#include <stdbool.h>
#include "player_event_translation.h"
#include "actor_event_translation.h"
#include "workshop_event_translation.h"
#include "dialogue_quest_event_translation.h"
#include "live_hook_pipeline_stub.h"
#include "hook_bringup_config.h"
#include "commonlibf4_hook_install_simulator.h"
#include "hook_attempt_result.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CommonLibF4PlayerHookArgs {
    PlayerHostEvent event;
} CommonLibF4PlayerHookArgs;

typedef struct CommonLibF4WorkshopHookArgs {
    WorkshopHostEvent event;
} CommonLibF4WorkshopHookArgs;

typedef struct CommonLibF4ActorHookArgs {
    ActorHostEvent event;
} CommonLibF4ActorHookArgs;

typedef struct CommonLibF4DialogueHookArgs {
    DialogueHostLineEvent event;
} CommonLibF4DialogueHookArgs;

typedef struct CommonLibF4QuestHookArgs {
    QuestHostObjectiveEvent event;
} CommonLibF4QuestHookArgs;

void clf4_hook_adapter_init(void);
void clf4_hook_adapter_init_mode(HookBringupMode mode);
void clf4_hook_adapter_init_mode_with_plan(HookBringupMode mode, const HookInstallPlan* plan);
bool clf4_submit_player_hook(const CommonLibF4PlayerHookArgs* args);
bool clf4_submit_workshop_hook(const CommonLibF4WorkshopHookArgs* args);
bool clf4_submit_actor_hook(const CommonLibF4ActorHookArgs* args);
bool clf4_submit_dialogue_hook(const CommonLibF4DialogueHookArgs* args);
bool clf4_submit_quest_hook(const CommonLibF4QuestHookArgs* args);
HookDispatchStats clf4_hook_adapter_stats(void);
unsigned int clf4_hook_adapter_get_last_bringup_result_count(void);
bool clf4_hook_adapter_get_last_bringup_result(unsigned int index, HookAttemptResult* out_result);

#ifdef __cplusplus
}
#endif
