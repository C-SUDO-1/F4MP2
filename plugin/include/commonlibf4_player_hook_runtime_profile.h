#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "commonlibf4_entrypoint_stub.h"
#include "commonlibf4_player_hook_detour_site_prototype.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CommonLibF4PlayerHookRuntimeProfile {
    const char* name;
    const char* prototype_name;
    uint32_t required_runtime_version;
    bool exact_runtime_required;
    bool editor_supported;
} CommonLibF4PlayerHookRuntimeProfile;

typedef struct CommonLibF4PlayerHookRuntimeProfileResolution {
    const CommonLibF4PlayerHookRuntimeProfile* profile;
    const CommonLibF4PlayerHookDetourSitePrototype* prototype_site;
    const char* profile_name;
    uint32_t observed_runtime_version;
    uint32_t required_runtime_version;
    bool runtime_supported;
    bool build_signature_supported;
} CommonLibF4PlayerHookRuntimeProfileResolution;

const CommonLibF4PlayerHookRuntimeProfile* clf4_phrp_default_profile(
    const CommonLibF4PlayerHookDetourSitePrototype* prototype_site
);

bool clf4_phrprof_resolve(
    const CommonLibF4PlayerHookDetourSitePrototype* prototype_site,
    const F4SEInterfaceMock* f4se,
    CommonLibF4PlayerHookRuntimeProfileResolution* out_resolution
);

#ifdef __cplusplus
}
#endif
