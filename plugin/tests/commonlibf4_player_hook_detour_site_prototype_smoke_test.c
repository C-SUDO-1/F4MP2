#include <assert.h>
#include <string.h>

#include "commonlibf4_player_hook_detour_site_prototype.h"

int main(void) {
    const CommonLibF4PlayerHookDetourSitePrototype* proto = clf4_phdsp_default();
    CommonLibF4PlayerHookDetourSitePrototypeResolution resolution;
    CommonLibF4AddressProvider provider;
    F4SEInterfaceMock ok = { 0x010A03D8u, 0, 0 };
    F4SEInterfaceMock old = { 0x010A03D7u, 0, 0 };

    assert(proto != NULL);
    assert(strcmp(proto->name, "player_character_update_before_input") == 0);
    assert(strcmp(proto->update_symbol_name, "PlayerCharacter::Update") == 0);
    assert(strcmp(proto->input_symbol_name, "PlayerControls::Update") == 0);

    provider = clf4_address_provider_make_fixed(true, true, true, true, true, true, true, true);
    assert(clf4_phdsp_resolve(proto, &ok, &provider, &resolution));
    assert(resolution.prototype_site == proto);
    assert(strcmp(resolution.update_symbol_name, "PlayerCharacter::Update") == 0);
    assert(strcmp(resolution.input_symbol_name, "PlayerControls::Update") == 0);
    assert(resolution.candidate_resolution.valid);
    assert(resolution.candidate_resolution.runtime_supported);
    assert(resolution.candidate_resolution.update_resolved);
    assert(resolution.candidate_resolution.input_resolved);

    provider = clf4_address_provider_make_fixed(true, false, true, true, true, true, true, true);
    assert(clf4_phdsp_resolve(proto, &ok, &provider, &resolution));
    assert(resolution.candidate_resolution.update_resolved);
    assert(!resolution.candidate_resolution.input_resolved);

    assert(clf4_phdsp_resolve(proto, &old, &provider, &resolution));
    assert(!resolution.candidate_resolution.runtime_supported);
    return 0;
}
