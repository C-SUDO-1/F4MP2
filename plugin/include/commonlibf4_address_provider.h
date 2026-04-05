#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum CommonLibF4AddressKey {
    CLF4_ADDR_PLAYER_UPDATE = 0,
    CLF4_ADDR_PLAYER_INPUT  = 1,
    CLF4_ADDR_ACTOR_UPDATE  = 2,
    CLF4_ADDR_ACTOR_DAMAGE  = 3,
    CLF4_ADDR_WORKSHOP_PLACE = 4,
    CLF4_ADDR_WORKSHOP_SCRAP = 5,
    CLF4_ADDR_DIALOGUE_OBSERVER = 6,
    CLF4_ADDR_QUEST_OBSERVER = 7,
    CLF4_ADDR_COUNT         = 8
} CommonLibF4AddressKey;

typedef enum CommonLibF4AddressSource {
    CLF4_ADDR_SOURCE_UNRESOLVED = 0,
    CLF4_ADDR_SOURCE_STUB,
    CLF4_ADDR_SOURCE_ADDRESS_LIBRARY,
    CLF4_ADDR_SOURCE_PATTERN_SCAN,
    CLF4_ADDR_SOURCE_MANUAL_OFFSET,
    CLF4_ADDR_SOURCE_TRUSTED_PROFILE,
    CLF4_ADDR_SOURCE_UNKNOWN
} CommonLibF4AddressSource;

typedef struct CommonLibF4AddressResolutionInfo {
    bool resolved;
    uintptr_t addr;
    CommonLibF4AddressSource source;
    bool executable_memory_valid;
} CommonLibF4AddressResolutionInfo;

typedef struct CommonLibF4ExactAddressEntry {
    bool available;
    uintptr_t addr;
    CommonLibF4AddressSource source;
} CommonLibF4ExactAddressEntry;

typedef struct CommonLibF4AddressProvider {
    bool (*resolve)(void* userdata, CommonLibF4AddressKey key, uintptr_t* out_addr);
    void* userdata;
    bool (*describe)(void* userdata, CommonLibF4AddressKey key, CommonLibF4AddressResolutionInfo* out_info);
} CommonLibF4AddressProvider;

bool clf4_address_provider_resolve(
    const CommonLibF4AddressProvider* provider,
    CommonLibF4AddressKey key,
    uintptr_t* out_addr
);

bool clf4_address_provider_describe(
    const CommonLibF4AddressProvider* provider,
    CommonLibF4AddressKey key,
    CommonLibF4AddressResolutionInfo* out_info
);

bool clf4_address_provider_is_executable_address(uintptr_t addr);

CommonLibF4AddressProvider clf4_address_provider_make_fixed(
    bool player_update_available,
    bool player_input_available,
    bool actor_update_available,
    bool actor_damage_available,
    bool workshop_place_available,
    bool workshop_scrap_available,
    bool dialogue_observer_available,
    bool quest_observer_available
);

CommonLibF4AddressProvider clf4_address_provider_make_exact(
    const CommonLibF4ExactAddressEntry* entries,
    uint32_t entry_count
);

#ifdef __cplusplus
}
#endif
