#pragma once

#include "commonlibf4_entrypoint_stub.h"
#include "commonlibf4_address_provider.h"
#include "commonlibf4_player_hook_detour_site_candidate.h"
#include "commonlibf4_player_hook_detour_site_prototype.h"
#include "commonlibf4_player_hook_runtime_profile.h"
#include "commonlibf4_player_hook_patch_policy.h"
#include "hook_attempt_result.h"

#ifdef __cplusplus
extern "C" {
#endif

HookAttemptResult clf4_attempt_install_player_hook_stub(
    const F4SEInterfaceMock* f4se,
    bool address_available,
    bool patch_allowed,
    const CommonLibF4PlayerHookPatchPolicy* policy
);

HookAttemptResult clf4_attempt_install_player_hook_with_provider(
    const F4SEInterfaceMock* f4se,
    const CommonLibF4AddressProvider* provider,
    bool patch_allowed,
    const CommonLibF4PlayerHookPatchPolicy* policy
);

HookAttemptResult clf4_attempt_install_player_hook_with_prototype(
    const F4SEInterfaceMock* f4se,
    const CommonLibF4AddressProvider* provider,
    const CommonLibF4PlayerHookDetourSitePrototype* prototype_site,
    bool patch_allowed,
    const CommonLibF4PlayerHookPatchPolicy* policy
);

HookAttemptResult clf4_attempt_install_player_hook_with_candidate_descriptor(
    const F4SEInterfaceMock* f4se,
    const CommonLibF4AddressProvider* provider,
    const CommonLibF4PlayerHookDetourSiteCandidateDescriptor* descriptor,
    bool patch_allowed,
    const CommonLibF4PlayerHookPatchPolicy* policy
);

#ifdef __cplusplus
}
#endif
