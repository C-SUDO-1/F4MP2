#include <assert.h>
#include <string.h>

#include "commonlibf4_player_hook_runtime_profile.h"

int main(void) {
    CommonLibF4PlayerHookRuntimeProfileResolution res;
    F4SEInterfaceMock latest = { 0x010B00BFu, 0, 0 };
    F4SEInterfaceMock legacy = { 0x010A03D8u, 0, 0 };
    F4SEInterfaceMock unsupported = { 0x010B00C0u, 0, 0 };

    assert(clf4_phrprof_resolve(clf4_phdsp_default(), &latest, &res));
    assert(res.profile_name != NULL);
    assert(strcmp(res.profile_name, "fo4_1_11_191_player_character_update_before_input") == 0);
    assert(res.runtime_supported);
    assert(res.build_signature_supported);
    assert(res.observed_runtime_version == 0x010B00BFu);
    assert(res.required_runtime_version == 0x010B00BFu);

    assert(clf4_phrprof_resolve(clf4_phdsp_default(), &legacy, &res));
    assert(strcmp(res.profile_name, "fo4_1_10_984_player_character_update_before_input") == 0);
    assert(res.runtime_supported);
    assert(res.build_signature_supported);
    assert(res.observed_runtime_version == 0x010A03D8u);
    assert(res.required_runtime_version == 0x010A03D8u);

    assert(!clf4_phrprof_resolve(clf4_phdsp_default(), &unsupported, &res));
    assert(strcmp(res.profile_name, "fo4_1_11_191_player_character_update_before_input") == 0);
    assert(res.runtime_supported);
    assert(!res.build_signature_supported);
    assert(res.observed_runtime_version == 0x010B00C0u);
    assert(res.required_runtime_version == 0x010B00BFu);
    return 0;
}
