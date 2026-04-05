#include "hook_bringup_config.h"

HookBringupConfig hook_bringup_config_for_mode(HookBringupMode mode) {
    HookBringupConfig cfg;
    cfg.mode = mode;
    cfg.enable_player = false;
    cfg.enable_actor = false;
    cfg.enable_workshop = false;
    cfg.enable_dialogue_quest = false;

    switch (mode) {
        case HOOK_BRINGUP_BASIC_SYNC:
            cfg.enable_player = true;
            cfg.enable_workshop = true;
            break;
        case HOOK_BRINGUP_VANILLA_MIRROR:
            cfg.enable_player = true;
            cfg.enable_actor = true;
            cfg.enable_workshop = true;
            cfg.enable_dialogue_quest = true;
            break;
        case HOOK_BRINGUP_NONE:
        default:
            break;
    }

    return cfg;
}

void hook_bringup_apply_config(const HookBringupConfig* cfg) {
    if (!cfg) return;

    hook_install_registry_reset();
    if (cfg->enable_player) hook_install_registry_install(HOOK_FAMILY_PLAYER);
    if (cfg->enable_actor) hook_install_registry_install(HOOK_FAMILY_ACTOR);
    if (cfg->enable_workshop) hook_install_registry_install(HOOK_FAMILY_WORKSHOP);
    if (cfg->enable_dialogue_quest) hook_install_registry_install(HOOK_FAMILY_DIALOGUE_QUEST);
}

void hook_bringup_apply_mode(HookBringupMode mode) {
    HookBringupConfig cfg = hook_bringup_config_for_mode(mode);
    hook_bringup_apply_config(&cfg);
}
