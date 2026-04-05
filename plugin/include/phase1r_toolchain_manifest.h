#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "commonlibf4_player_hook_runtime_profile.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Phase1RToolchainManifest {
    const char* runtime_profile_name;
    uint32_t fallout_runtime_version_hex;
    const char* fallout_runtime_version_text;
    const char* f4se_build_text;
    const char* commonlibf4_resource;
    bool requires_address_library;
} Phase1RToolchainManifest;

const Phase1RToolchainManifest* phase1r_toolchain_manifest_current(void);
bool phase1r_toolchain_manifest_matches_runtime_profile(const CommonLibF4PlayerHookRuntimeProfileResolution* runtime_profile);

#ifdef __cplusplus
}
#endif
