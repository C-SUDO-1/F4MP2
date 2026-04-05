#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum HookFamily {
    HOOK_FAMILY_PLAYER = 0,
    HOOK_FAMILY_ACTOR = 1,
    HOOK_FAMILY_WORKSHOP = 2,
    HOOK_FAMILY_DIALOGUE_QUEST = 3,
    HOOK_FAMILY_COUNT = 4
} HookFamily;

typedef enum HookInstallState {
    HOOK_INSTALL_NOT_ATTEMPTED = 0,
    HOOK_INSTALL_INSTALLED = 1,
    HOOK_INSTALL_FAILED = 2,
    HOOK_INSTALL_PARTIAL = 3
} HookInstallState;

typedef enum HookInstallError {
    HOOK_INSTALL_ERR_NONE = 0,
    HOOK_INSTALL_ERR_UNSUPPORTED_RUNTIME = 1,
    HOOK_INSTALL_ERR_ADDRESS_MISSING = 2,
    HOOK_INSTALL_ERR_PATCH_REJECTED = 3,
    HOOK_INSTALL_ERR_DEPENDENCY_MISSING = 4,
    HOOK_INSTALL_ERR_UNKNOWN = 255
} HookInstallError;

typedef struct HookInstallRecord {
    HookInstallState state;
    HookInstallError error;
} HookInstallRecord;

void hook_install_registry_reset(void);
bool hook_install_registry_install(HookFamily family);
bool hook_install_registry_mark_state(HookFamily family, HookInstallState state, HookInstallError error);
bool hook_install_registry_uninstall(HookFamily family);
bool hook_install_registry_is_installed(HookFamily family);
HookInstallState hook_install_registry_get_state(HookFamily family);
HookInstallError hook_install_registry_get_error(HookFamily family);
HookInstallRecord hook_install_registry_get_record(HookFamily family);
unsigned int hook_install_registry_installed_count(void);
unsigned int hook_install_registry_attempted_count(void);
unsigned int hook_install_registry_failed_count(void);
unsigned int hook_install_registry_partial_count(void);

#ifdef __cplusplus
}
#endif
