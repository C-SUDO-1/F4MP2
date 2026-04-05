#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "commonlibf4_address_provider.h"
#include "commonlibf4_entrypoint_stub.h"
#include "commonlibf4_player_hook_detour_site_candidate.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CommonLibF4PlayerHookDetourSitePrototype {
    const char* name;
    const char* update_symbol_name;
    const char* input_symbol_name;
    CommonLibF4AddressKey update_key;
    CommonLibF4AddressKey input_key;
    uint32_t minimum_runtime_version;
    bool require_update_address;
    bool require_input_address;
    bool editor_supported;
} CommonLibF4PlayerHookDetourSitePrototype;

typedef struct CommonLibF4PlayerHookDetourSitePrototypeResolution {
    const CommonLibF4PlayerHookDetourSitePrototype* prototype_site;
    CommonLibF4PlayerHookDetourSiteCandidateResolution candidate_resolution;
    const char* update_symbol_name;
    const char* input_symbol_name;
} CommonLibF4PlayerHookDetourSitePrototypeResolution;

const CommonLibF4PlayerHookDetourSitePrototype* clf4_phdsp_default(void);
const CommonLibF4PlayerHookDetourSiteCandidateDescriptor* clf4_phdsp_descriptor(
    const CommonLibF4PlayerHookDetourSitePrototype* prototype_site
);
bool clf4_phdsp_resolve(
    const CommonLibF4PlayerHookDetourSitePrototype* prototype_site,
    const F4SEInterfaceMock* f4se,
    const CommonLibF4AddressProvider* provider,
    CommonLibF4PlayerHookDetourSitePrototypeResolution* out_resolution
);

#ifdef __cplusplus
}
#endif
