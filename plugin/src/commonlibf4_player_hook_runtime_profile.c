#include "commonlibf4_player_hook_runtime_profile.h"

#include <string.h>

#define F4MP_RUNTIME_1_10_984 0x010A3D80u
#define F4MP_RUNTIME_1_11_191 0x010B0BF0u
#define F4MP_RUNTIME_1_10_984_LEGACY 0x010A03D8u
#define F4MP_RUNTIME_1_11_191_LEGACY 0x010B00BFu

static bool runtime_matches(unsigned int observed, unsigned int canonical) {
    switch (canonical) {
        case F4MP_RUNTIME_1_10_984:
            return observed == F4MP_RUNTIME_1_10_984 || observed == F4MP_RUNTIME_1_10_984_LEGACY;
        case F4MP_RUNTIME_1_11_191:
            return observed == F4MP_RUNTIME_1_11_191 || observed == F4MP_RUNTIME_1_11_191_LEGACY;
        default:
            return observed == canonical;
    }
}

static const CommonLibF4PlayerHookRuntimeProfile g_profile_controls_1_10_984 = {
    "fo4_1_10_984_movement_playercontrols_input_path",
    "movement_playercontrols_input_path_rejected",
    F4MP_RUNTIME_1_10_984,
    true,
    false
};

static const CommonLibF4PlayerHookRuntimeProfile g_profile_controls_1_11_191 = {
    "fo4_1_11_191_movement_playercontrols_input_path",
    "movement_playercontrols_input_path_rejected",
    F4MP_RUNTIME_1_11_191,
    true,
    false
};

static const CommonLibF4PlayerHookRuntimeProfile g_profile_before_input_1_10_984 = {
    "fo4_1_10_984_playercharacter_update_before_input",
    "movement_playercharacter_before_input_path",
    F4MP_RUNTIME_1_10_984,
    true,
    false
};

static const CommonLibF4PlayerHookRuntimeProfile g_profile_before_input_1_11_191 = {
    "fo4_1_11_191_playercharacter_update_before_input",
    "movement_playercharacter_before_input_path",
    F4MP_RUNTIME_1_11_191,
    true,
    false
};

static const CommonLibF4PlayerHookRuntimeProfile g_profile_after_input_1_10_984 = {
    "fo4_1_10_984_playercharacter_update_after_input",
    "movement_playercharacter_after_input_path",
    F4MP_RUNTIME_1_10_984,
    true,
    false
};

static const CommonLibF4PlayerHookRuntimeProfile g_profile_after_input_1_11_191 = {
    "fo4_1_11_191_playercharacter_update_after_input",
    "movement_playercharacter_after_input_path",
    F4MP_RUNTIME_1_11_191,
    true,
    false
};

static bool prototype_name_matches(const CommonLibF4PlayerHookDetourSitePrototype* prototype_site, const char* name) {
    return prototype_site && prototype_site->name && name && strcmp(prototype_site->name, name) == 0;
}

static const CommonLibF4PlayerHookRuntimeProfile* select_profile(
    const CommonLibF4PlayerHookDetourSitePrototype* prototype_site,
    const F4SEInterfaceMock* f4se
) {
    const bool before_input = prototype_name_matches(prototype_site, "player_character_update_before_input_manual_entry") ||
                              prototype_name_matches(prototype_site, "movement_playercharacter_before_input_path");
    const bool after_input = prototype_name_matches(prototype_site, "player_character_update_after_input_manual_entry") ||
                             prototype_name_matches(prototype_site, "movement_playercharacter_after_input_path");
    const bool controls = prototype_name_matches(prototype_site, "player_controls_update_manual_entry") ||
                          prototype_name_matches(prototype_site, "movement_playercontrols_input_path_rejected") ||
                          prototype_name_matches(prototype_site, "movement_playercontrols_input_path");

    if (f4se) {
        if (runtime_matches(f4se->runtime_version, F4MP_RUNTIME_1_11_191)) {
            if (before_input) return &g_profile_before_input_1_11_191;
            if (after_input) return &g_profile_after_input_1_11_191;
            if (controls) return &g_profile_controls_1_11_191;
            return &g_profile_before_input_1_11_191;
        }
        if (runtime_matches(f4se->runtime_version, F4MP_RUNTIME_1_10_984)) {
            if (before_input) return &g_profile_before_input_1_10_984;
            if (after_input) return &g_profile_after_input_1_10_984;
            if (controls) return &g_profile_controls_1_10_984;
            return &g_profile_before_input_1_10_984;
        }
    }
    return &g_profile_before_input_1_11_191;
}

const CommonLibF4PlayerHookRuntimeProfile* clf4_phrp_default_profile(
    const CommonLibF4PlayerHookDetourSitePrototype* prototype_site
) {
    return select_profile(prototype_site, NULL);
}

bool clf4_phrprof_resolve(
    const CommonLibF4PlayerHookDetourSitePrototype* prototype_site,
    const F4SEInterfaceMock* f4se,
    CommonLibF4PlayerHookRuntimeProfileResolution* out_resolution
) {
    CommonLibF4PlayerHookRuntimeProfileResolution tmp;
    const CommonLibF4PlayerHookDetourSitePrototype* effective_prototype =
        prototype_site ? prototype_site : clf4_phdsp_default();
    const CommonLibF4PlayerHookRuntimeProfile* profile = select_profile(effective_prototype, f4se);

    memset(&tmp, 0, sizeof(tmp));
    tmp.profile = profile;
    tmp.prototype_site = effective_prototype;
    tmp.profile_name = profile ? profile->name : NULL;
    tmp.observed_runtime_version = f4se ? f4se->runtime_version : 0;
    tmp.required_runtime_version = profile ? profile->required_runtime_version : 0;
    tmp.runtime_supported = (f4se && !f4se->is_editor && profile != NULL);
    if (tmp.runtime_supported) {
        if (profile->exact_runtime_required) {
            tmp.build_signature_supported = runtime_matches(f4se->runtime_version, profile->required_runtime_version);
        } else {
            tmp.build_signature_supported = (f4se->runtime_version >= profile->required_runtime_version);
        }
    }
    if (out_resolution) {
        *out_resolution = tmp;
    }
    return tmp.build_signature_supported;
}
