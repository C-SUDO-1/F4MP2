#include "commonlibf4_hook_adapter_stub.h"
#include "commonlibf4_hook_attempt_executor.h"
#include "hook_bringup_report_accessor.h"

void clf4_hook_adapter_init_mode_with_plan(HookBringupMode mode, const HookInstallPlan* plan) {
    HookAttemptResult results[HOOK_FAMILY_COUNT];
    unsigned int count = 0;
    F4SEInterfaceMock runtime = { 0x010A03D8u, 0, 0 };
    lhps_init();
    if (plan) {
        (void)hook_attempt_executor_apply_mode_plan(mode, plan, results, HOOK_FAMILY_COUNT, &count);
    } else {
        (void)hook_attempt_executor_apply_mode_runtime_stub(&runtime, mode, results, HOOK_FAMILY_COUNT, &count);
    }
    hook_bringup_report_store(results, count);
}

void clf4_hook_adapter_init_mode(HookBringupMode mode) {
    clf4_hook_adapter_init_mode_with_plan(mode, nullptr);
}

void clf4_hook_adapter_init(void) {
    clf4_hook_adapter_init_mode(HOOK_BRINGUP_VANILLA_MIRROR);
}

bool clf4_submit_player_hook(const CommonLibF4PlayerHookArgs* args) {
    if (!args) return false;
    return lhps_submit_player_event(&args->event);
}

bool clf4_submit_workshop_hook(const CommonLibF4WorkshopHookArgs* args) {
    if (!args) return false;
    return lhps_submit_workshop_event(&args->event);
}

HookDispatchStats clf4_hook_adapter_stats(void) {
    return lhps_get_stats();
}

bool clf4_submit_actor_hook(const CommonLibF4ActorHookArgs* args) {
    if (!args) return false;
    return lhps_submit_actor_event(&args->event);
}

bool clf4_submit_dialogue_hook(const CommonLibF4DialogueHookArgs* args) {
    if (!args) return false;
    return lhps_submit_dialogue_event(&args->event);
}

bool clf4_submit_quest_hook(const CommonLibF4QuestHookArgs* args) {
    if (!args) return false;
    return lhps_submit_quest_event(&args->event);
}

unsigned int clf4_hook_adapter_get_last_bringup_result_count(void) {
    return hook_bringup_report_count();
}

bool clf4_hook_adapter_get_last_bringup_result(unsigned int index, HookAttemptResult* out_result) {
    return hook_bringup_report_get_result(index, out_result);
}
