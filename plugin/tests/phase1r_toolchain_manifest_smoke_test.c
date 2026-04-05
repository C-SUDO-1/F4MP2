#include <assert.h>
#include <string.h>

#include "phase1r_toolchain_manifest.h"
#include "commonlibf4_player_hook_runtime_profile.h"

int main(void) {
    CommonLibF4PlayerHookRuntimeProfileResolution res;
    const Phase1RToolchainManifest* m = phase1r_toolchain_manifest_current();
    assert(m != 0);
    assert(strcmp(m->runtime_profile_name, "fo4_1_11_191_player_character_update_before_input") == 0);
    assert(strcmp(m->fallout_runtime_version_text, "1.11.191") == 0);
    assert(strcmp(m->f4se_build_text, "0.7.7") == 0);
    assert(strcmp(m->commonlibf4_resource, "CommonLibF4") == 0);
    assert(m->requires_address_library);

    memset(&res, 0, sizeof(res));
    res.profile_name = m->runtime_profile_name;
    res.required_runtime_version = m->fallout_runtime_version_hex;
    res.observed_runtime_version = m->fallout_runtime_version_hex;
    res.build_signature_supported = true;
    assert(phase1r_toolchain_manifest_matches_runtime_profile(&res));

    memset(&res, 0, sizeof(res));
    res.profile_name = m->runtime_profile_name;
    res.required_runtime_version = m->fallout_runtime_version_hex - 1u;
    res.observed_runtime_version = m->fallout_runtime_version_hex - 1u;
    res.build_signature_supported = false;
    assert(!phase1r_toolchain_manifest_matches_runtime_profile(&res));
    return 0;
}
