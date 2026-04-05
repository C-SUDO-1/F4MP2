#include <assert.h>
#include <stddef.h>

#include "commonlibf4_player_hook_patch_policy.h"

int main(void) {
    const CommonLibF4PlayerHookPatchPolicy* dry = clf4_phpp_default_dry_run();
    const CommonLibF4PlayerHookPatchPolicy* armed = clf4_phpp_default_armed();

    assert(dry != NULL);
    assert(armed != NULL);
    assert(clf4_phpp_is_dry_run(dry));
    assert(!clf4_phpp_is_armed(dry));
    assert(!clf4_phpp_patch_step_allowed(dry, true));

    assert(clf4_phpp_is_armed(armed));
    assert(!clf4_phpp_is_dry_run(armed));
    assert(clf4_phpp_patch_step_allowed(armed, true));
    assert(!clf4_phpp_patch_step_allowed(armed, false));
    return 0;
}
