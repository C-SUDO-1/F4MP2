#include "fo4_proxy_actor_spawn_contract.h"

#include <string.h>

static Fo4ProxyActorSpawnContractState g_contract;

void fpasc_reset(void) {
    memset(&g_contract, 0, sizeof(g_contract));
}

void fpasc_configure(const Fo4ProxyActorSpawnContractConfig* config) {
    Fo4ProxyActorSpawnContractConfig effective;
    memset(&effective, 0, sizeof(effective));
    effective.default_proxy_base_form_id = 0x7777u;
    effective.suppress_self_spawn = true;
    effective.require_base_form_id = true;
    if (config) {
        effective = *config;
    }
    g_contract.config = effective;
    g_contract.configured = true;
}

bool fpasc_build_create_spec(const ProxyPlayerSpawnSpec* spec, Fo4ProxyActorCreateSpec* out_create_spec) {
    Fo4ProxyActorCreateSpec create_spec;
    uint32_t base_form_id;

    if (!g_contract.configured) {
        Fo4ProxyActorSpawnContractConfig cfg;
        memset(&cfg, 0, sizeof(cfg));
        cfg.default_proxy_base_form_id = 0x7777u;
        cfg.suppress_self_spawn = true;
        cfg.require_base_form_id = true;
        fpasc_configure(&cfg);
    }

    if (!spec || !out_create_spec || spec->player_id == 0) {
        g_contract.reject_count++;
        return false;
    }
    if (g_contract.config.suppress_self_spawn &&
        g_contract.config.local_player_id != 0 &&
        spec->player_id == g_contract.config.local_player_id) {
        g_contract.reject_count++;
        g_contract.last_suppressed_self_spawn = true;
        g_contract.last_player_id = spec->player_id;
        return false;
    }

    memset(&create_spec, 0, sizeof(create_spec));
    base_form_id = spec->proxy_base_form_id ? spec->proxy_base_form_id : g_contract.config.default_proxy_base_form_id;
    if (g_contract.config.require_base_form_id && base_form_id == 0) {
        g_contract.reject_count++;
        g_contract.last_player_id = spec->player_id;
        g_contract.last_used_default_base_form = false;
        g_contract.last_suppressed_self_spawn = false;
        return false;
    }

    create_spec.player_id = spec->player_id;
    create_spec.proxy_net_id = spec->proxy_net_id;
    create_spec.proxy_base_form_id = base_form_id;
    create_spec.position = spec->position;
    create_spec.rotation = spec->rotation;
    create_spec.equipped_form_id = spec->equipped_form_id;

    *out_create_spec = create_spec;
    g_contract.plan_count++;
    g_contract.last_player_id = spec->player_id;
    g_contract.last_used_default_base_form = (spec->proxy_base_form_id == 0 && base_form_id != 0);
    g_contract.last_suppressed_self_spawn = false;
    g_contract.last_create_spec = create_spec;
    return true;
}

bool fpasc_spawn_via_driver(const ProxyPlayerSpawnSpec* spec, uint64_t actor_handle) {
    Fo4ProxyActorCreateSpec create_spec;
    if (!fpasc_build_create_spec(spec, &create_spec)) {
        return false;
    }
    create_spec.actor_handle = actor_handle;
    if (!fpad_create(&create_spec)) {
        return false;
    }
    g_contract.spawn_count++;
    g_contract.last_create_spec = create_spec;
    return true;
}

Fo4ProxyActorSpawnContractState fpasc_state(void) {
    return g_contract;
}
