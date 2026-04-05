#include <assert.h>
#include <string.h>

#include "commonlibf4_player_hook_detour_site_candidate.h"

int main(void) {
    const CommonLibF4PlayerHookDetourSiteCandidateDescriptor* desc = clf4_phdsc_default();
    CommonLibF4PlayerHookDetourSiteCandidateResolution resolution;
    CommonLibF4AddressProvider provider;
    F4SEInterfaceMock ok = { 0x010A03D8u, 0, 0 };
    F4SEInterfaceMock old = { 0x010A03D7u, 0, 0 };

    assert(desc != NULL);
    assert(desc->name != NULL);
    assert(strcmp(desc->name, "player_character_update_before_input") == 0);
    assert(desc->require_update_address);
    assert(desc->require_input_address);
    assert(desc->minimum_runtime_version == 0x010A03D8u);

    provider = clf4_address_provider_make_fixed(true, true, true, true, true, true, true, true);
    memset(&resolution, 0, sizeof(resolution));
    assert(clf4_phdsc_resolve(desc, &ok, &provider, &resolution));
    assert(resolution.valid);
    assert(resolution.runtime_supported);
    assert(resolution.update_resolved);
    assert(resolution.input_resolved);
    assert(resolution.update_addr != 0);
    assert(resolution.input_addr != 0);

    provider = clf4_address_provider_make_fixed(true, false, true, true, true, true, true, true);
    memset(&resolution, 0, sizeof(resolution));
    assert(clf4_phdsc_resolve(desc, &ok, &provider, &resolution));
    assert(resolution.valid);
    assert(resolution.runtime_supported);
    assert(resolution.update_resolved);
    assert(!resolution.input_resolved);

    memset(&resolution, 0, sizeof(resolution));
    assert(clf4_phdsc_resolve(desc, &old, &provider, &resolution));
    assert(resolution.valid);
    assert(!resolution.runtime_supported);
    return 0;
}
