#include "commonlibf4_hook_install_registry.h"

static HookInstallRecord g_records[HOOK_FAMILY_COUNT];

static bool valid_family(HookFamily family) {
    return family >= 0 && family < HOOK_FAMILY_COUNT;
}

extern "C" void hook_install_registry_reset(void) {
    for (int i = 0; i < HOOK_FAMILY_COUNT; ++i) {
        g_records[i].state = HOOK_INSTALL_NOT_ATTEMPTED;
        g_records[i].error = HOOK_INSTALL_ERR_NONE;
    }
}

extern "C" bool hook_install_registry_mark_state(HookFamily family, HookInstallState state, HookInstallError error) {
    if (!valid_family(family)) return false;
    g_records[(int)family].state = state;
    g_records[(int)family].error = error;
    return true;
}

extern "C" bool hook_install_registry_install(HookFamily family) {
    return hook_install_registry_mark_state(family, HOOK_INSTALL_INSTALLED, HOOK_INSTALL_ERR_NONE);
}

extern "C" bool hook_install_registry_uninstall(HookFamily family) {
    return hook_install_registry_mark_state(family, HOOK_INSTALL_NOT_ATTEMPTED, HOOK_INSTALL_ERR_NONE);
}

extern "C" bool hook_install_registry_is_installed(HookFamily family) {
    return hook_install_registry_get_state(family) == HOOK_INSTALL_INSTALLED;
}

extern "C" HookInstallState hook_install_registry_get_state(HookFamily family) {
    if (!valid_family(family)) return HOOK_INSTALL_FAILED;
    return g_records[(int)family].state;
}

extern "C" HookInstallError hook_install_registry_get_error(HookFamily family) {
    if (!valid_family(family)) return HOOK_INSTALL_ERR_UNKNOWN;
    return g_records[(int)family].error;
}

extern "C" HookInstallRecord hook_install_registry_get_record(HookFamily family) {
    HookInstallRecord fallback = { HOOK_INSTALL_FAILED, HOOK_INSTALL_ERR_UNKNOWN };
    if (!valid_family(family)) return fallback;
    return g_records[(int)family];
}

extern "C" unsigned int hook_install_registry_installed_count(void) {
    unsigned int count = 0;
    for (int i = 0; i < HOOK_FAMILY_COUNT; ++i) if (g_records[i].state == HOOK_INSTALL_INSTALLED) ++count;
    return count;
}

extern "C" unsigned int hook_install_registry_attempted_count(void) {
    unsigned int count = 0;
    for (int i = 0; i < HOOK_FAMILY_COUNT; ++i) if (g_records[i].state != HOOK_INSTALL_NOT_ATTEMPTED) ++count;
    return count;
}

extern "C" unsigned int hook_install_registry_failed_count(void) {
    unsigned int count = 0;
    for (int i = 0; i < HOOK_FAMILY_COUNT; ++i) if (g_records[i].state == HOOK_INSTALL_FAILED) ++count;
    return count;
}

extern "C" unsigned int hook_install_registry_partial_count(void) {
    unsigned int count = 0;
    for (int i = 0; i < HOOK_FAMILY_COUNT; ++i) if (g_records[i].state == HOOK_INSTALL_PARTIAL) ++count;
    return count;
}
