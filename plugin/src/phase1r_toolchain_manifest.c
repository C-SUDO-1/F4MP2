#include "phase1r_toolchain_manifest.h"
#include <string.h>

#define F4MP_RUNTIME_1_11_191 0x010B0BF0u

static const Phase1RToolchainManifest g_manifest = {
    "fo4_1_11_191_player_character_update_before_input",
    F4MP_RUNTIME_1_11_191,
    "1.11.191",
    "0.7.7",
    "CommonLibF4",
    true
};

const Phase1RToolchainManifest* phase1r_toolchain_manifest_current(void) {
    return &g_manifest;
}

bool phase1r_toolchain_manifest_matches_runtime_profile(const CommonLibF4PlayerHookRuntimeProfileResolution* runtime_profile) {
    if (!runtime_profile) return false;
    return runtime_profile->build_signature_supported &&
           runtime_profile->required_runtime_version == g_manifest.fallout_runtime_version_hex &&
           runtime_profile->profile_name != 0 &&
           strcmp(runtime_profile->profile_name, g_manifest.runtime_profile_name) == 0;
}
