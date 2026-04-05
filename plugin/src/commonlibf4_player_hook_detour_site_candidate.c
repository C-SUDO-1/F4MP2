#include "commonlibf4_player_hook_detour_site_candidate.h"

#include <string.h>

static const CommonLibF4PlayerHookDetourSiteCandidateDescriptor g_default_descriptor = {
    "movement_playercharacter_before_input_path",
    CLF4_ADDR_PLAYER_UPDATE,
    CLF4_ADDR_PLAYER_INPUT,
    0x010B00BFu,
    true,
    false,
    false
};

const CommonLibF4PlayerHookDetourSiteCandidateDescriptor* clf4_phdsc_default(void) {
    return &g_default_descriptor;
}

bool clf4_phdsc_resolve(
    const CommonLibF4PlayerHookDetourSiteCandidateDescriptor* descriptor,
    const F4SEInterfaceMock* f4se,
    const CommonLibF4AddressProvider* provider,
    CommonLibF4PlayerHookDetourSiteCandidateResolution* out_resolution
) {
    const CommonLibF4PlayerHookDetourSiteCandidateDescriptor* effective = descriptor ? descriptor : clf4_phdsc_default();
    CommonLibF4PlayerHookDetourSiteCandidateResolution tmp;

    memset(&tmp, 0, sizeof(tmp));
    tmp.descriptor = effective;
    tmp.valid = (effective != NULL && effective->name != NULL);
    if (tmp.valid) {
        tmp.runtime_supported = (f4se != NULL) &&
            (effective->editor_supported || !f4se->is_editor) &&
            (f4se->runtime_version >= effective->minimum_runtime_version);
        if (provider && effective->require_update_address) {
            tmp.update_resolved = clf4_address_provider_resolve(provider, effective->update_key, &tmp.update_addr);
        } else if (!effective->require_update_address) {
            tmp.update_resolved = true;
        }
        if (provider && effective->require_input_address) {
            tmp.input_resolved = clf4_address_provider_resolve(provider, effective->input_key, &tmp.input_addr);
        } else if (!effective->require_input_address) {
            tmp.input_resolved = true;
        }
    }
    if (out_resolution) {
        *out_resolution = tmp;
    }
    return tmp.valid;
}
