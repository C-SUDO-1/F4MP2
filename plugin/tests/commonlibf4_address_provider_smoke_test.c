#include <assert.h>
#include <stdint.h>
#include "commonlibf4_address_provider.h"

int main(void) {
    uintptr_t addr = 0;
    CommonLibF4AddressProvider p = clf4_address_provider_make_fixed(true, false, true, false, true, false, true, false);
    assert(clf4_address_provider_resolve(&p, CLF4_ADDR_PLAYER_UPDATE, &addr));
    assert(addr == 0x1000u);
    assert(!clf4_address_provider_resolve(&p, CLF4_ADDR_PLAYER_INPUT, &addr));
    assert(clf4_address_provider_resolve(&p, CLF4_ADDR_ACTOR_UPDATE, &addr));
    assert(!clf4_address_provider_resolve(&p, CLF4_ADDR_ACTOR_DAMAGE, &addr));
    assert(clf4_address_provider_resolve(&p, CLF4_ADDR_WORKSHOP_PLACE, &addr));
    assert(!clf4_address_provider_resolve(&p, CLF4_ADDR_WORKSHOP_SCRAP, &addr));
    return 0;
}
