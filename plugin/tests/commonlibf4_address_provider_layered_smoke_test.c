#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "commonlibf4_address_provider.h"

int main(void) {
    CommonLibF4AddressProvider fixed;
    CommonLibF4AddressProvider layered;
    CommonLibF4ExactAddressEntry overrides[CLF4_ADDR_COUNT];
    CommonLibF4AddressResolutionInfo info;

    memset(overrides, 0, sizeof(overrides));
    fixed = clf4_address_provider_make_fixed(true, false, false, false, false, false, false, false);
    overrides[CLF4_ADDR_PLAYER_INPUT].available = true;
    overrides[CLF4_ADDR_PLAYER_INPUT].addr = 0x12345000u;
    overrides[CLF4_ADDR_PLAYER_INPUT].source = CLF4_ADDR_SOURCE_MANUAL_OFFSET;
    layered = clf4_address_provider_make_layered(&fixed, overrides, CLF4_ADDR_COUNT);

    assert(clf4_address_provider_describe(&layered, CLF4_ADDR_PLAYER_UPDATE, &info));
    assert(info.addr == 0x1000u);
    assert(info.source == CLF4_ADDR_SOURCE_STUB);

    assert(clf4_address_provider_describe(&layered, CLF4_ADDR_PLAYER_INPUT, &info));
    assert(info.addr == 0x12345000u);
    assert(info.source == CLF4_ADDR_SOURCE_MANUAL_OFFSET);

    setenv("F4MP_MANUAL_PLAYER_UPDATE_ADDR", "0xABCDEF00", 1);
    memset(overrides, 0, sizeof(overrides));
    assert(clf4_address_provider_read_env_overrides(overrides, CLF4_ADDR_COUNT));
    assert(overrides[CLF4_ADDR_PLAYER_UPDATE].available);
    assert(overrides[CLF4_ADDR_PLAYER_UPDATE].addr == (uintptr_t)0xABCDEF00u);
    assert(overrides[CLF4_ADDR_PLAYER_UPDATE].source == CLF4_ADDR_SOURCE_MANUAL_OFFSET);
    unsetenv("F4MP_MANUAL_PLAYER_UPDATE_ADDR");
    return 0;
}
