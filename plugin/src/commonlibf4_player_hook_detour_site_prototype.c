#include "commonlibf4_player_hook_detour_site_prototype.h"

#include <string.h>

static const CommonLibF4PlayerHookDetourSitePrototype g_default_prototype = {
    "movement_playercharacter_before_input_path",
    "PlayerCharacter::Update",
    "PlayerControls::Update",
    CLF4_ADDR_PLAYER_UPDATE,
    CLF4_ADDR_PLAYER_INPUT,
    0x010B00BFu,
    true,
    false,
    false
};

const CommonLibF4PlayerHookDetourSitePrototype* clf4_phdsp_default(void) {
    return &g_default_prototype;
}

const CommonLibF4PlayerHookDetourSiteCandidateDescriptor* clf4_phdsp_descriptor(
    const CommonLibF4PlayerHookDetourSitePrototype* prototype_site
) {
    static CommonLibF4PlayerHookDetourSiteCandidateDescriptor desc;
    const CommonLibF4PlayerHookDetourSitePrototype* effective = prototype_site ? prototype_site : clf4_phdsp_default();
    memset(&desc, 0, sizeof(desc));
    desc.name = effective->name;
    desc.update_key = effective->update_key;
    desc.input_key = effective->input_key;
    desc.minimum_runtime_version = effective->minimum_runtime_version;
    desc.require_update_address = effective->require_update_address;
    desc.require_input_address = effective->require_input_address;
    desc.editor_supported = effective->editor_supported;
    return &desc;
}

bool clf4_phdsp_resolve(
    const CommonLibF4PlayerHookDetourSitePrototype* prototype_site,
    const F4SEInterfaceMock* f4se,
    const CommonLibF4AddressProvider* provider,
    CommonLibF4PlayerHookDetourSitePrototypeResolution* out_resolution
) {
    CommonLibF4PlayerHookDetourSitePrototypeResolution tmp;
    const CommonLibF4PlayerHookDetourSitePrototype* effective = prototype_site ? prototype_site : clf4_phdsp_default();

    memset(&tmp, 0, sizeof(tmp));
    tmp.prototype_site = effective;
    tmp.update_symbol_name = effective ? effective->update_symbol_name : NULL;
    tmp.input_symbol_name = effective ? effective->input_symbol_name : NULL;
    clf4_phdsc_resolve(clf4_phdsp_descriptor(effective), f4se, provider, &tmp.candidate_resolution);
    if (out_resolution) {
        *out_resolution = tmp;
    }
    return tmp.candidate_resolution.valid;
}
