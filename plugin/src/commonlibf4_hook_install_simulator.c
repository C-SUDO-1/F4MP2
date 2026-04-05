#include "commonlibf4_hook_install_simulator.h"
#include "hook_bringup_config.h"

static HookInstallRecord rec(HookInstallState s, HookInstallError e) {
    HookInstallRecord r;
    r.state = s;
    r.error = e;
    return r;
}

HookInstallPlan hook_install_plan_success_for_mode(HookBringupMode mode) {
    HookBringupConfig cfg = hook_bringup_config_for_mode(mode);
    HookInstallPlan plan;
    plan.player = cfg.enable_player ? rec(HOOK_INSTALL_INSTALLED, HOOK_INSTALL_ERR_NONE) : rec(HOOK_INSTALL_NOT_ATTEMPTED, HOOK_INSTALL_ERR_NONE);
    plan.actor = cfg.enable_actor ? rec(HOOK_INSTALL_INSTALLED, HOOK_INSTALL_ERR_NONE) : rec(HOOK_INSTALL_NOT_ATTEMPTED, HOOK_INSTALL_ERR_NONE);
    plan.workshop = cfg.enable_workshop ? rec(HOOK_INSTALL_INSTALLED, HOOK_INSTALL_ERR_NONE) : rec(HOOK_INSTALL_NOT_ATTEMPTED, HOOK_INSTALL_ERR_NONE);
    plan.dialogue_quest = cfg.enable_dialogue_quest ? rec(HOOK_INSTALL_INSTALLED, HOOK_INSTALL_ERR_NONE) : rec(HOOK_INSTALL_NOT_ATTEMPTED, HOOK_INSTALL_ERR_NONE);
    return plan;
}

void hook_install_simulator_apply_plan(const HookInstallPlan* plan, HookBringupMode mode) {
    HookBringupConfig cfg;
    if (!plan) return;
    cfg = hook_bringup_config_for_mode(mode);
    hook_install_registry_reset();
    if (cfg.enable_player) hook_install_registry_mark_state(HOOK_FAMILY_PLAYER, plan->player.state, plan->player.error);
    if (cfg.enable_actor) hook_install_registry_mark_state(HOOK_FAMILY_ACTOR, plan->actor.state, plan->actor.error);
    if (cfg.enable_workshop) hook_install_registry_mark_state(HOOK_FAMILY_WORKSHOP, plan->workshop.state, plan->workshop.error);
    if (cfg.enable_dialogue_quest) hook_install_registry_mark_state(HOOK_FAMILY_DIALOGUE_QUEST, plan->dialogue_quest.state, plan->dialogue_quest.error);
}
