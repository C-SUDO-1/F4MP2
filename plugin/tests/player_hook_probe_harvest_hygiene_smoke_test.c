#include "player_hook_probe_harvest.h"
#include "hook_discovery_report.h"
#include <assert.h>
#include <string.h>

int main(void) {
    HookDiscoveryReport report;
    PlayerHookProbeHarvestSnapshot snap;
    hdr_reset(&report);
    report.probe_family = "PlayerCharacter::Update";
    report.probe_reason = "unresolved_required_address";
    report.probe_mode = "probe_only";
    phh_reset();
    phh_begin_unresolved_session(&report);
    phh_note_explicit_callsite("live_candidate_activate", 0x1000u, 0x0800u, false, false, false);
    snap = phh_snapshot();
    assert(snap.active);
    assert(snap.sample_count == 1u);
    assert(snap.accepted_sample_count == 0u);
    return 0;
}
