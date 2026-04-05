#include <assert.h>
#include <string.h>

#include "player_hook_discovery.h"

typedef struct TestProviderData {
    CommonLibF4AddressResolutionInfo infos[CLF4_ADDR_COUNT];
} TestProviderData;

static bool test_resolve(void* userdata, CommonLibF4AddressKey key, uintptr_t* out_addr) {
    TestProviderData* data = (TestProviderData*)userdata;
    if (!data || !out_addr || key >= CLF4_ADDR_COUNT || !data->infos[key].resolved) {
        return false;
    }
    *out_addr = data->infos[key].addr;
    return true;
}

static bool test_describe(void* userdata, CommonLibF4AddressKey key, CommonLibF4AddressResolutionInfo* out_info) {
    TestProviderData* data = (TestProviderData*)userdata;
    if (!data || !out_info || key >= CLF4_ADDR_COUNT) {
        return false;
    }
    *out_info = data->infos[key];
    return out_info->resolved;
}

int main(void) {
    TestProviderData data;
    CommonLibF4AddressProvider provider;
    F4SEInterfaceMock f4se = { 0x010B0BF0u, 0, 0 };
    HookDiscoveryReport report;
    HookDiscoverySequencingInput seq;

    memset(&data, 0, sizeof(data));
    data.infos[CLF4_ADDR_PLAYER_INPUT].resolved = true;
    data.infos[CLF4_ADDR_PLAYER_INPUT].addr = 0x7FF600002000ull;
    data.infos[CLF4_ADDR_PLAYER_INPUT].source = CLF4_ADDR_SOURCE_MANUAL_OFFSET;
    data.infos[CLF4_ADDR_PLAYER_INPUT].executable_memory_valid = true;
    data.infos[CLF4_ADDR_ACTOR_UPDATE].resolved = true;
    data.infos[CLF4_ADDR_ACTOR_UPDATE].addr = 0x7FF600003000ull;
    data.infos[CLF4_ADDR_ACTOR_UPDATE].source = CLF4_ADDR_SOURCE_MANUAL_OFFSET;
    data.infos[CLF4_ADDR_ACTOR_UPDATE].executable_memory_valid = true;

    provider.resolve = test_resolve;
    provider.describe = test_describe;
    provider.userdata = &data;

    memset(&report, 0, sizeof(report));
    assert(phd_run_player_discovery(&f4se, &provider, &report));
    assert(report.selected_candidate_id == PLAYER_HOOK_CANDIDATE_PLAYER_CONTROLS_UPDATE);
    assert(report.eligible_candidate_count == 2u);

    memset(&seq, 0, sizeof(seq));
    seq.installed_but_dead_candidate_id = PLAYER_HOOK_CANDIDATE_PLAYER_CONTROLS_UPDATE;
    assert(phd_run_player_discovery_sequence(&f4se, &provider, &seq, &report));
    assert(report.selected_candidate_id == PLAYER_HOOK_CANDIDATE_ACTOR_UPDATE_PLAYER_FILTERED);
    assert(report.assessments[2].sequence_status == HOOK_SEQUENCE_STATUS_SKIPPED_PREVIOUSLY_DEAD);
    assert(report.assessments[3].sequence_status == HOOK_SEQUENCE_STATUS_ELIGIBLE);
    return 0;
}
