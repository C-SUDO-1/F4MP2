#include "commonlibf4_address_provider.h"

#include <string.h>

#ifdef _WIN32
#include <windows.h>
#endif

typedef struct FixedAddressProviderData {
    bool player_update_available;
    bool player_input_available;
    bool actor_update_available;
    bool actor_damage_available;
    bool workshop_place_available;
    bool workshop_scrap_available;
    bool dialogue_observer_available;
    bool quest_observer_available;
} FixedAddressProviderData;

typedef struct ExactAddressProviderData {
    CommonLibF4ExactAddressEntry entries[CLF4_ADDR_COUNT];
} ExactAddressProviderData;

static bool fixed_resolve(void* userdata, CommonLibF4AddressKey key, uintptr_t* out_addr) {
    FixedAddressProviderData* data = (FixedAddressProviderData*)userdata;
    if (!data || !out_addr) return false;
    switch (key) {
        case CLF4_ADDR_PLAYER_UPDATE:
            if (!data->player_update_available) return false;
            *out_addr = 0x1000u; return true;
        case CLF4_ADDR_PLAYER_INPUT:
            if (!data->player_input_available) return false;
            *out_addr = 0x2000u; return true;
        case CLF4_ADDR_ACTOR_UPDATE:
            if (!data->actor_update_available) return false;
            *out_addr = 0x3000u; return true;
        case CLF4_ADDR_ACTOR_DAMAGE:
            if (!data->actor_damage_available) return false;
            *out_addr = 0x4000u; return true;
        case CLF4_ADDR_WORKSHOP_PLACE:
            if (!data->workshop_place_available) return false;
            *out_addr = 0x5000u; return true;
        case CLF4_ADDR_WORKSHOP_SCRAP:
            if (!data->workshop_scrap_available) return false;
            *out_addr = 0x6000u; return true;
        case CLF4_ADDR_DIALOGUE_OBSERVER:
            if (!data->dialogue_observer_available) return false;
            *out_addr = 0x7000u; return true;
        case CLF4_ADDR_QUEST_OBSERVER:
            if (!data->quest_observer_available) return false;
            *out_addr = 0x8000u; return true;
        default:
            return false;
    }
}

static bool fixed_describe(void* userdata, CommonLibF4AddressKey key, CommonLibF4AddressResolutionInfo* out_info) {
    uintptr_t addr = 0u;
    if (!out_info) {
        return false;
    }
    memset(out_info, 0, sizeof(*out_info));
    out_info->resolved = fixed_resolve(userdata, key, &addr);
    out_info->addr = addr;
    out_info->source = out_info->resolved ? CLF4_ADDR_SOURCE_STUB : CLF4_ADDR_SOURCE_UNRESOLVED;
    out_info->executable_memory_valid = false;
    return out_info->resolved;
}

static bool exact_resolve(void* userdata, CommonLibF4AddressKey key, uintptr_t* out_addr) {
    ExactAddressProviderData* data = (ExactAddressProviderData*)userdata;
    if (!data || !out_addr || key >= CLF4_ADDR_COUNT) {
        return false;
    }
    if (!data->entries[key].available) {
        return false;
    }
    *out_addr = data->entries[key].addr;
    return true;
}

static bool exact_describe(void* userdata, CommonLibF4AddressKey key, CommonLibF4AddressResolutionInfo* out_info) {
    ExactAddressProviderData* data = (ExactAddressProviderData*)userdata;
    if (!data || !out_info || key >= CLF4_ADDR_COUNT) {
        return false;
    }
    memset(out_info, 0, sizeof(*out_info));
    out_info->resolved = data->entries[key].available;
    out_info->addr = data->entries[key].addr;
    out_info->source = data->entries[key].available ? data->entries[key].source : CLF4_ADDR_SOURCE_UNRESOLVED;
    out_info->executable_memory_valid =
        data->entries[key].available && clf4_address_provider_is_executable_address(data->entries[key].addr);
    return out_info->resolved;
}

bool clf4_address_provider_is_executable_address(uintptr_t addr) {
#ifdef _WIN32
    MEMORY_BASIC_INFORMATION mbi;
    SIZE_T rc;
    DWORD prot;
    if (addr == 0u) {
        return false;
    }
    rc = VirtualQuery((LPCVOID)addr, &mbi, sizeof(mbi));
    if (rc == 0 || mbi.State != MEM_COMMIT) {
        return false;
    }
    prot = mbi.Protect & 0xFFu;
    return prot == PAGE_EXECUTE ||
           prot == PAGE_EXECUTE_READ ||
           prot == PAGE_EXECUTE_READWRITE ||
           prot == PAGE_EXECUTE_WRITECOPY;
#else
    return false;
#endif
}

bool clf4_address_provider_resolve(
    const CommonLibF4AddressProvider* provider,
    CommonLibF4AddressKey key,
    uintptr_t* out_addr
) {
    if (!provider || !provider->resolve) return false;
    return provider->resolve(provider->userdata, key, out_addr);
}

bool clf4_address_provider_describe(
    const CommonLibF4AddressProvider* provider,
    CommonLibF4AddressKey key,
    CommonLibF4AddressResolutionInfo* out_info
) {
    uintptr_t addr = 0u;
    if (!out_info) {
        return false;
    }
    memset(out_info, 0, sizeof(*out_info));
    if (!provider) {
        out_info->source = CLF4_ADDR_SOURCE_UNRESOLVED;
        return false;
    }
    if (provider->describe) {
        return provider->describe(provider->userdata, key, out_info);
    }
    if (!provider->resolve || !provider->resolve(provider->userdata, key, &addr)) {
        out_info->source = CLF4_ADDR_SOURCE_UNRESOLVED;
        return false;
    }
    out_info->resolved = true;
    out_info->addr = addr;
    out_info->source = CLF4_ADDR_SOURCE_UNKNOWN;
    out_info->executable_memory_valid = clf4_address_provider_is_executable_address(addr);
    return true;
}

CommonLibF4AddressProvider clf4_address_provider_make_fixed(
    bool player_update_available,
    bool player_input_available,
    bool actor_update_available,
    bool actor_damage_available,
    bool workshop_place_available,
    bool workshop_scrap_available,
    bool dialogue_observer_available,
    bool quest_observer_available
) {
    static FixedAddressProviderData data_slots[16];
    static unsigned int next_slot = 0;
    FixedAddressProviderData* data = &data_slots[next_slot++ % 16u];
    data->player_update_available = player_update_available;
    data->player_input_available = player_input_available;
    data->actor_update_available = actor_update_available;
    data->actor_damage_available = actor_damage_available;
    data->workshop_place_available = workshop_place_available;
    data->workshop_scrap_available = workshop_scrap_available;
    data->dialogue_observer_available = dialogue_observer_available;
    data->quest_observer_available = quest_observer_available;
    {
        CommonLibF4AddressProvider p;
        p.resolve = fixed_resolve;
        p.userdata = data;
        p.describe = fixed_describe;
        return p;
    }
}

CommonLibF4AddressProvider clf4_address_provider_make_exact(
    const CommonLibF4ExactAddressEntry* entries,
    uint32_t entry_count
) {
    static ExactAddressProviderData data_slots[16];
    static unsigned int next_slot = 0;
    uint32_t i;
    ExactAddressProviderData* data = &data_slots[next_slot++ % 16u];
    memset(data, 0, sizeof(*data));
    if (entries) {
        for (i = 0; i < entry_count && i < CLF4_ADDR_COUNT; ++i) {
            data->entries[i] = entries[i];
        }
    }
    {
        CommonLibF4AddressProvider p;
        p.resolve = exact_resolve;
        p.userdata = data;
        p.describe = exact_describe;
        return p;
    }
}
