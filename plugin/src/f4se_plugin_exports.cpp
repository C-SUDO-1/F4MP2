#include "commonlibf4_entrypoint_stub.h"
#include "commonlibf4_address_provider.h"
#include "commonlibf4_player_hook_patch_policy.h"
#include "commonlibf4_player_hook_detour_site_prototype.h"
#include "engine_bridge_stub.h"
#include "hook_bringup_config.h"
#include "phase1r_real_local_hook_wiring.h"
#include "phase1r_real_hook_arming_gate.h"
#include "plugin_exports.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#endif

namespace {
using UInt32 = std::uint32_t;
using UInt8 = std::uint8_t;

constexpr UInt32 MakeExeVersion(UInt32 major, UInt32 minor, UInt32 build)
{
    return ((major & 0xFFu) << 24) | ((minor & 0xFFu) << 16) | ((build & 0xFFFu) << 4);
}

constexpr UInt32 RUNTIME_VERSION_1_10_984 = MakeExeVersion(1, 10, 984);
constexpr UInt32 RUNTIME_VERSION_1_11_191 = MakeExeVersion(1, 11, 191);

struct F4SEInterface
{
    UInt32 f4seVersion;
    UInt32 runtimeVersion;
    UInt32 editorVersion;
    UInt32 isEditor;
    void* (*QueryInterface)(UInt32);
    UInt32 (*GetPluginHandle)(void);
    UInt32 (*GetReleaseIndex)(void);
    const void* (*GetPluginInfo)(const char*);
    const char* (*GetSaveFolderName)(void);
};

struct PluginInfo
{
    enum { kInfoVersion = 1 };
    UInt32 infoVersion;
    const char* name;
    UInt32 version;
};

struct F4SEPluginVersionData
{
    enum { kVersion = 1 };

    UInt32 dataVersion;
    UInt32 pluginVersion;
    char name[256];
    char author[256];
    UInt32 addressIndependence;
    UInt32 structureIndependence;
    UInt32 compatibleVersions[16];
    UInt32 seVersionRequired;
    UInt32 reservedNonBreaking;
    UInt32 reservedBreaking;
    UInt8 reserved[512];
};

static bool g_bridge_initialized = false;
static bool g_query_prearm_attempted = false;
static bool g_query_prearm_active = false;

static void f4mp_write_wiring_trace(const Phase1RRealLocalHookWiringState* s);

static F4SEInterfaceMock ToMock(const F4SEInterface* f4se)
{
    F4SEInterfaceMock mock{};
    if (f4se) {
        mock.runtime_version = f4se->runtimeVersion;
        mock.editor_version = f4se->editorVersion;
        mock.is_editor = f4se->isEditor;
    }
    return mock;
}

static void f4mp_bridge_init_once()
{
    if (!g_bridge_initialized) {
        ebstub_init();
        g_bridge_initialized = true;
    }
}

static void f4mp_bridge_shutdown_once()
{
    if (g_bridge_initialized) {
        ebstub_shutdown();
        g_bridge_initialized = false;
    }
}

static uintptr_t f4mp_get_runtime_base()
{
#ifdef _WIN32
    return reinterpret_cast<uintptr_t>(GetModuleHandleW(nullptr));
#else
    return 0u;
#endif
}

enum class F4MPManualPivotKind {
    Controls,
    BeforeInput,
    AfterInput,
    ActorFiltered
};

struct F4MPManualPivotSelection {
    CommonLibF4AddressProvider provider;
    const CommonLibF4PlayerHookDetourSitePrototype* site;
    const char* callback_source_symbol;
    const char* callback_guard_symbol;
    const char* pivot_name;
    uintptr_t update_addr;
    uintptr_t input_addr;
    const char* update_source_label;
    const char* input_source_label;
    bool trusted_source_present;
    bool trusted_source_runtime_match;
    bool trusted_source_build_match;
    bool trusted_source_accept;
    const char* trusted_source_kind;
    uintptr_t trusted_source_addr;
};

static const CommonLibF4PlayerHookDetourSitePrototype g_player_controls_manual_prototype = {
    "player_controls_update_manual_entry",
    "",
    "CreatePlayerControlHandlers",
    CLF4_ADDR_PLAYER_UPDATE,
    CLF4_ADDR_PLAYER_INPUT,
    RUNTIME_VERSION_1_11_191,
    false,
    true,
    false
};

static const CommonLibF4PlayerHookDetourSitePrototype g_update_before_input_manual_prototype = {
    "player_character_update_before_input_manual_entry",
    "PlayerCharacter::Update",
    "PlayerControls::Update",
    CLF4_ADDR_PLAYER_UPDATE,
    CLF4_ADDR_PLAYER_INPUT,
    RUNTIME_VERSION_1_11_191,
    true,
    false,
    false
};

static const CommonLibF4PlayerHookDetourSitePrototype g_update_after_input_manual_prototype = {
    "player_character_update_after_input_manual_entry",
    "PlayerCharacter::Update",
    "PostInput",
    CLF4_ADDR_PLAYER_UPDATE,
    CLF4_ADDR_PLAYER_INPUT,
    RUNTIME_VERSION_1_11_191,
    true,
    false,
    false
};

static const CommonLibF4PlayerHookDetourSitePrototype g_actor_update_player_filtered_manual_prototype = {
    "actor_update_player_filtered_manual_entry",
    "Actor::Update",
    "PlayerFilter",
    CLF4_ADDR_ACTOR_UPDATE,
    CLF4_ADDR_PLAYER_INPUT,
    RUNTIME_VERSION_1_11_191,
    true,
    false,
    false
};

static uintptr_t f4mp_read_env_u64(const char* name)
{
#ifdef _WIN32
    char buffer[64]{};
    const DWORD len = GetEnvironmentVariableA(name, buffer, static_cast<DWORD>(sizeof(buffer)));
    if (len == 0u || len >= sizeof(buffer)) {
        return 0u;
    }
    return static_cast<uintptr_t>(std::strtoull(buffer, nullptr, 0));
#else
    (void)name;
    return 0u;
#endif
}

static uintptr_t f4mp_resolve_env_address(const char* addr_name, const char* rva_name, uintptr_t runtime_base)
{
    const uintptr_t absolute_addr = f4mp_read_env_u64(addr_name);
    if (absolute_addr != 0u) {
        return absolute_addr;
    }
    if (runtime_base != 0u) {
        const uintptr_t rva = f4mp_read_env_u64(rva_name);
        if (rva != 0u) {
            return runtime_base + rva;
        }
    }
    return 0u;
}

static const char* f4mp_env_resolution_source(uintptr_t runtime_base, const char* addr_name, const char* rva_name)
{
    if (f4mp_read_env_u64(addr_name) != 0u) {
        return "manual_address";
    }
    if (runtime_base != 0u && f4mp_read_env_u64(rva_name) != 0u) {
        return "manual_rva";
    }
    return "unresolved";
}

static bool f4mp_read_env_string(const char* name, char* out_buffer, size_t out_cap)
{
#ifdef _WIN32
    DWORD len;
    if (!name || !out_buffer || out_cap == 0u) {
        return false;
    }
    len = GetEnvironmentVariableA(name, out_buffer, static_cast<DWORD>(out_cap));
    if (len == 0u || len >= out_cap) {
        if (out_cap > 0u) {
            out_buffer[0] = '\0';
        }
        return false;
    }
    return true;
#else
    (void)name;
    if (out_buffer && out_cap > 0u) {
        out_buffer[0] = '\0';
    }
    return false;
#endif
}

static void f4mp_trim_trailing(char* text)
{
    size_t len;
    if (!text) {
        return;
    }
    len = std::strlen(text);
    while (len > 0u) {
        const char ch = text[len - 1u];
        if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n') {
            text[len - 1u] = '\0';
            --len;
        } else {
            break;
        }
    }
}

static char* f4mp_trim_leading(char* text)
{
    while (text && (*text == ' ' || *text == '\t')) {
        ++text;
    }
    return text;
}

static bool f4mp_parse_key_value(char* line, char** out_key, char** out_value)
{
    char* eq;
    char* key;
    char* value;

    if (!line || !out_key || !out_value) {
        return false;
    }

    f4mp_trim_trailing(line);
    key = f4mp_trim_leading(line);
    if (*key == '\0' || *key == '#' || *key == ';') {
        return false;
    }

    eq = std::strchr(key, '=');
    if (!eq) {
        return false;
    }
    *eq = '\0';
    value = eq + 1;

    f4mp_trim_trailing(key);
    value = f4mp_trim_leading(value);
    f4mp_trim_trailing(value);

    *out_key = key;
    *out_value = value;
    return *key != '\0';
}

static const char* f4mp_default_trusted_profile_path()
{
    return "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Fallout 4 377160\\Data\\F4SE\\Plugins\\f4mp_trusted_addresses.ini";
}

struct F4MPTrustedAddressProfile {
    bool present;
    bool runtime_match;
    bool build_match;
    bool accepted;
    const char* source_kind;
    uintptr_t update_addr;
    uintptr_t input_addr;
    uintptr_t actor_addr;
};

static F4MPTrustedAddressProfile f4mp_load_trusted_profile(F4MPManualPivotKind kind, UInt32 runtime_version, uintptr_t runtime_base)
{
    F4MPTrustedAddressProfile profile{};
#ifdef _WIN32
    char path_buffer[MAX_PATH]{};
    char line[512];
    char pivot_value[64]{};
    UInt32 file_runtime_version = 0u;
    uintptr_t update_addr = 0u;
    uintptr_t input_addr = 0u;
    uintptr_t actor_addr = 0u;
    uintptr_t update_rva = 0u;
    uintptr_t input_rva = 0u;
    uintptr_t actor_rva = 0u;
    bool have_path;
    bool pivot_match;
    FILE* f = nullptr;

    profile.source_kind = "trusted_profile";
    have_path = f4mp_read_env_string("F4MP_TRUSTED_ADDRESS_FILE", path_buffer, sizeof(path_buffer));
    if (!have_path) {
        std::snprintf(path_buffer, sizeof(path_buffer), "%s", f4mp_default_trusted_profile_path());
    }

    fopen_s(&f, path_buffer, "r");
    if (!f) {
        return profile;
    }

    profile.present = true;
    while (std::fgets(line, sizeof(line), f)) {
        char* key = nullptr;
        char* value = nullptr;
        if (!f4mp_parse_key_value(line, &key, &value)) {
            continue;
        }
        if (_stricmp(key, "runtime_version") == 0 || _stricmp(key, "required_runtime_version") == 0) {
            file_runtime_version = static_cast<UInt32>(std::strtoul(value, nullptr, 0));
        } else if (_stricmp(key, "pivot") == 0 || _stricmp(key, "pivot_name") == 0) {
            std::snprintf(pivot_value, sizeof(pivot_value), "%s", value);
        } else if (_stricmp(key, "update_addr") == 0) {
            update_addr = static_cast<uintptr_t>(std::strtoull(value, nullptr, 0));
        } else if (_stricmp(key, "update_rva") == 0) {
            update_rva = static_cast<uintptr_t>(std::strtoull(value, nullptr, 0));
        } else if (_stricmp(key, "input_addr") == 0) {
            input_addr = static_cast<uintptr_t>(std::strtoull(value, nullptr, 0));
        } else if (_stricmp(key, "input_rva") == 0) {
            input_rva = static_cast<uintptr_t>(std::strtoull(value, nullptr, 0));
        } else if (_stricmp(key, "actor_addr") == 0) {
            actor_addr = static_cast<uintptr_t>(std::strtoull(value, nullptr, 0));
        } else if (_stricmp(key, "actor_rva") == 0) {
            actor_rva = static_cast<uintptr_t>(std::strtoull(value, nullptr, 0));
        }
    }
    std::fclose(f);

    if (update_addr == 0u && runtime_base != 0u && update_rva != 0u) {
        update_addr = runtime_base + update_rva;
        profile.source_kind = "trusted_profile_rva";
    }
    if (input_addr == 0u && runtime_base != 0u && input_rva != 0u) {
        input_addr = runtime_base + input_rva;
        if (profile.source_kind == nullptr || std::strcmp(profile.source_kind, "trusted_profile") == 0) {
            profile.source_kind = "trusted_profile_rva";
        }
    }
    if (actor_addr == 0u && runtime_base != 0u && actor_rva != 0u) {
        actor_addr = runtime_base + actor_rva;
        if (profile.source_kind == nullptr || std::strcmp(profile.source_kind, "trusted_profile") == 0) {
            profile.source_kind = "trusted_profile_rva";
        }
    }

    if (profile.source_kind == nullptr) {
        profile.source_kind = "trusted_profile";
    }

    profile.runtime_match = (file_runtime_version != 0u) && (file_runtime_version == runtime_version);
    profile.build_match = profile.runtime_match;

    if (pivot_value[0] == '\0') {
        pivot_match = true;
    } else {
        const char* expected_pivot = "controls";
        switch (kind) {
        case F4MPManualPivotKind::BeforeInput:
            expected_pivot = "before_input";
            break;
        case F4MPManualPivotKind::AfterInput:
            expected_pivot = "after_input";
            break;
        case F4MPManualPivotKind::ActorFiltered:
            expected_pivot = "actor_filtered";
            break;
        case F4MPManualPivotKind::Controls:
        default:
            expected_pivot = "controls";
            break;
        }
        pivot_match = (_stricmp(pivot_value, expected_pivot) == 0);
    }

    profile.update_addr = update_addr;
    profile.input_addr = input_addr;
    profile.actor_addr = actor_addr;
    profile.accepted = profile.runtime_match && profile.build_match && pivot_match &&
        (profile.update_addr != 0u || profile.input_addr != 0u || profile.actor_addr != 0u);
#else
    (void)kind;
    (void)runtime_version;
    (void)runtime_base;
    profile.source_kind = "trusted_profile";
#endif
    return profile;
}

static F4MPManualPivotKind f4mp_read_manual_pivot_kind()
{
#ifdef _WIN32
    char buffer[64]{};
    const DWORD len = GetEnvironmentVariableA("F4MP_MANUAL_CANDIDATE", buffer, static_cast<DWORD>(sizeof(buffer)));
    if (len == 0u || len >= sizeof(buffer)) {
        return F4MPManualPivotKind::BeforeInput;
    }
    if (_stricmp(buffer, "before_input") == 0 || _stricmp(buffer, "update_before_input") == 0) {
        return F4MPManualPivotKind::BeforeInput;
    }
    if (_stricmp(buffer, "after_input") == 0 || _stricmp(buffer, "update_after_input") == 0) {
        return F4MPManualPivotKind::AfterInput;
    }
    if (_stricmp(buffer, "actor") == 0 || _stricmp(buffer, "actor_filtered") == 0 || _stricmp(buffer, "actor_update_player_filtered") == 0) {
        return F4MPManualPivotKind::ActorFiltered;
    }
#endif
    return F4MPManualPivotKind::Controls;
}

static const char* f4mp_manual_pivot_name(F4MPManualPivotKind kind)
{
    switch (kind) {
    case F4MPManualPivotKind::BeforeInput:
        return "before_input";
    case F4MPManualPivotKind::AfterInput:
        return "after_input";
    case F4MPManualPivotKind::ActorFiltered:
        return "actor_filtered";
    case F4MPManualPivotKind::Controls:
    default:
        return "controls";
    }
}

static F4MPManualPivotSelection f4mp_make_manual_pivot_selection(UInt32 runtime_version)
{
    F4MPManualPivotSelection out{};
    CommonLibF4ExactAddressEntry entries[CLF4_ADDR_COUNT]{};
    const F4MPManualPivotKind kind = f4mp_read_manual_pivot_kind();
    const F4MPTrustedAddressProfile trusted_profile = f4mp_load_trusted_profile(kind, runtime_version, runtime_base);
    const uintptr_t env_update_addr = f4mp_resolve_env_address("F4MP_MANUAL_UPDATE_ADDR", "F4MP_MANUAL_UPDATE_RVA", runtime_base);
    const uintptr_t env_input_addr = f4mp_resolve_env_address("F4MP_MANUAL_INPUT_ADDR", "F4MP_MANUAL_INPUT_RVA", runtime_base);
    const uintptr_t env_actor_addr = f4mp_resolve_env_address("F4MP_MANUAL_ACTOR_ADDR", "F4MP_MANUAL_ACTOR_RVA", runtime_base);
    const char* env_update_source = f4mp_env_resolution_source(runtime_base, "F4MP_MANUAL_UPDATE_ADDR", "F4MP_MANUAL_UPDATE_RVA");
    const char* env_input_source = f4mp_env_resolution_source(runtime_base, "F4MP_MANUAL_INPUT_ADDR", "F4MP_MANUAL_INPUT_RVA");
    const char* env_actor_source = f4mp_env_resolution_source(runtime_base, "F4MP_MANUAL_ACTOR_ADDR", "F4MP_MANUAL_ACTOR_RVA");

    out.pivot_name = f4mp_manual_pivot_name(kind);
    out.site = &g_player_controls_manual_prototype;
    out.callback_source_symbol = "CreatePlayerControlHandlers";
    out.callback_guard_symbol = "g_playerControls";
    out.update_addr = 0u;
    out.input_addr = 0u;
    out.update_source_label = "unresolved";
    out.input_source_label = "unresolved";
    out.trusted_source_present = trusted_profile.present;
    out.trusted_source_runtime_match = trusted_profile.runtime_match;
    out.trusted_source_build_match = trusted_profile.build_match;
    out.trusted_source_accept = trusted_profile.accepted;
    out.trusted_source_kind = trusted_profile.source_kind ? trusted_profile.source_kind : "trusted_profile";
    out.trusted_source_addr = 0u;

    if (runtime_base != 0u && runtime_version == RUNTIME_VERSION_1_11_191) {
        switch (kind) {
        case F4MPManualPivotKind::BeforeInput:
            out.site = &g_update_before_input_manual_prototype;
            out.callback_source_symbol = "PlayerCharacter::Update";
            out.callback_guard_symbol = "PlayerControls::Update";
            out.update_addr = env_update_addr != 0u ? env_update_addr : (trusted_profile.accepted ? trusted_profile.update_addr : 0u);
            out.input_addr = env_input_addr != 0u ? env_input_addr : (trusted_profile.accepted ? trusted_profile.input_addr : 0u);
            out.update_source_label = env_update_addr != 0u ? env_update_source : (trusted_profile.accepted && trusted_profile.update_addr != 0u ? out.trusted_source_kind : "unresolved");
            out.input_source_label = env_input_addr != 0u ? env_input_source : (trusted_profile.accepted && trusted_profile.input_addr != 0u ? out.trusted_source_kind : "unresolved");
            out.trusted_source_addr = trusted_profile.update_addr;
            if (out.update_addr != 0u) {
                entries[CLF4_ADDR_PLAYER_UPDATE].available = true;
                entries[CLF4_ADDR_PLAYER_UPDATE].addr = out.update_addr;
                entries[CLF4_ADDR_PLAYER_UPDATE].source = env_update_addr != 0u ? CLF4_ADDR_SOURCE_MANUAL_OFFSET : CLF4_ADDR_SOURCE_TRUSTED_PROFILE;
            }
            if (out.input_addr != 0u) {
                entries[CLF4_ADDR_PLAYER_INPUT].available = true;
                entries[CLF4_ADDR_PLAYER_INPUT].addr = out.input_addr;
                entries[CLF4_ADDR_PLAYER_INPUT].source = env_input_addr != 0u ? CLF4_ADDR_SOURCE_MANUAL_OFFSET : CLF4_ADDR_SOURCE_TRUSTED_PROFILE;
            }
            break;
        case F4MPManualPivotKind::AfterInput:
            out.site = &g_update_after_input_manual_prototype;
            out.callback_source_symbol = "PlayerCharacter::Update";
            out.callback_guard_symbol = "PostInput";
            out.update_addr = env_update_addr != 0u ? env_update_addr : (trusted_profile.accepted ? trusted_profile.update_addr : 0u);
            out.update_source_label = env_update_addr != 0u ? env_update_source : (trusted_profile.accepted && trusted_profile.update_addr != 0u ? out.trusted_source_kind : "unresolved");
            out.trusted_source_addr = trusted_profile.update_addr;
            if (out.update_addr != 0u) {
                entries[CLF4_ADDR_PLAYER_UPDATE].available = true;
                entries[CLF4_ADDR_PLAYER_UPDATE].addr = out.update_addr;
                entries[CLF4_ADDR_PLAYER_UPDATE].source = env_update_addr != 0u ? CLF4_ADDR_SOURCE_MANUAL_OFFSET : CLF4_ADDR_SOURCE_TRUSTED_PROFILE;
            }
            break;
        case F4MPManualPivotKind::ActorFiltered:
            out.site = &g_actor_update_player_filtered_manual_prototype;
            out.callback_source_symbol = "Actor::Update";
            out.callback_guard_symbol = "PlayerFilter";
            out.update_addr = env_actor_addr != 0u ? env_actor_addr : (trusted_profile.accepted ? trusted_profile.actor_addr : 0u);
            out.update_source_label = env_actor_addr != 0u ? env_actor_source : (trusted_profile.accepted && trusted_profile.actor_addr != 0u ? out.trusted_source_kind : "unresolved");
            out.trusted_source_addr = trusted_profile.actor_addr;
            if (out.update_addr != 0u) {
                entries[CLF4_ADDR_ACTOR_UPDATE].available = true;
                entries[CLF4_ADDR_ACTOR_UPDATE].addr = out.update_addr;
                entries[CLF4_ADDR_ACTOR_UPDATE].source = env_actor_addr != 0u ? CLF4_ADDR_SOURCE_MANUAL_OFFSET : CLF4_ADDR_SOURCE_TRUSTED_PROFILE;
            }
            break;
        case F4MPManualPivotKind::Controls:
        default:
            out.site = &g_player_controls_manual_prototype;
            out.callback_source_symbol = "CreatePlayerControlHandlers";
            out.callback_guard_symbol = "g_playerControls";
            out.input_addr = env_input_addr != 0u ? env_input_addr : (trusted_profile.accepted && trusted_profile.input_addr != 0u ? trusted_profile.input_addr : (runtime_base + 0x00DDC780u));
            out.input_source_label = env_input_addr != 0u ? env_input_source : (trusted_profile.accepted && trusted_profile.input_addr != 0u ? out.trusted_source_kind : "default_controls_fallback");
            out.trusted_source_addr = trusted_profile.input_addr;
            entries[CLF4_ADDR_PLAYER_INPUT].available = true;
            entries[CLF4_ADDR_PLAYER_INPUT].addr = out.input_addr;
            entries[CLF4_ADDR_PLAYER_INPUT].source = env_input_addr != 0u ? CLF4_ADDR_SOURCE_MANUAL_OFFSET : ((trusted_profile.accepted && trusted_profile.input_addr != 0u) ? CLF4_ADDR_SOURCE_TRUSTED_PROFILE : CLF4_ADDR_SOURCE_MANUAL_OFFSET);
            break;
        }
    }

    out.provider = clf4_address_provider_make_exact(entries, CLF4_ADDR_COUNT);
    return out;
}

static void f4mp_write_resolution_trace(const F4MPManualPivotSelection& selection, UInt32 runtime_version)
{
    const char* path =
        "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Fallout 4 377160\\Data\\F4SE\\Plugins\\f4mp_update_resolution_trace.txt";
    FILE* f = nullptr;
    fopen_s(&f, path, "w");
    if (!f) {
        return;
    }
    std::fprintf(f, "runtime_version=0x%08X\n", runtime_version);
    std::fprintf(f, "pivot_name=%s\n", selection.pivot_name ? selection.pivot_name : "");
    std::fprintf(f, "site_name=%s\n", selection.site && selection.site->name ? selection.site->name : "");
    std::fprintf(f, "update_addr=0x%p\n", (void*)selection.update_addr);
    std::fprintf(f, "input_addr=0x%p\n", (void*)selection.input_addr);
    std::fprintf(f, "update_resolution_source=%s\n", selection.update_source_label ? selection.update_source_label : "unresolved");
    std::fprintf(f, "input_resolution_source=%s\n", selection.input_source_label ? selection.input_source_label : "unresolved");
    std::fprintf(f, "trusted_source_present=%d\n", selection.trusted_source_present ? 1 : 0);
    std::fprintf(f, "trusted_source_kind=%s\n", selection.trusted_source_kind ? selection.trusted_source_kind : "trusted_profile");
    std::fprintf(f, "trusted_source_runtime_match=%d\n", selection.trusted_source_runtime_match ? 1 : 0);
    std::fprintf(f, "trusted_source_build_match=%d\n", selection.trusted_source_build_match ? 1 : 0);
    std::fprintf(f, "trusted_source_addr=0x%p\n", (void*)selection.trusted_source_addr);
    std::fprintf(f, "trusted_source_accept=%d\n", selection.trusted_source_accept ? 1 : 0);
    std::fprintf(f, "update_required=%d\n", selection.site && selection.site->require_update_address ? 1 : 0);
    std::fprintf(f, "input_required=%d\n", selection.site && selection.site->require_input_address ? 1 : 0);
    std::fclose(f);
}

static void f4mp_write_manual_pivot_trace(const F4MPManualPivotSelection& selection, UInt32 runtime_version)
{
    const char* path =
        "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Fallout 4 377160\\Data\\F4SE\\Plugins\\f4mp_manual_pivot_trace.txt";

    FILE* f = nullptr;
    fopen_s(&f, path, "w");
    if (!f) {
        return;
    }

    std::fprintf(f, "runtime_version=0x%08X\n", runtime_version);
    std::fprintf(f, "pivot_name=%s\n", selection.pivot_name ? selection.pivot_name : "");
    std::fprintf(f, "site_name=%s\n", selection.site && selection.site->name ? selection.site->name : "");
    std::fprintf(f, "callback_source=%s\n", selection.callback_source_symbol ? selection.callback_source_symbol : "");
    std::fprintf(f, "callback_guard=%s\n", selection.callback_guard_symbol ? selection.callback_guard_symbol : "");
    std::fprintf(f, "require_update=%d\n", selection.site && selection.site->require_update_address ? 1 : 0);
    std::fprintf(f, "require_input=%d\n", selection.site && selection.site->require_input_address ? 1 : 0);
    std::fprintf(f, "update_addr=0x%p\n", (void*)selection.update_addr);
    std::fprintf(f, "input_addr=0x%p\n", (void*)selection.input_addr);
    std::fprintf(f, "update_source=%s\n", selection.update_source_label ? selection.update_source_label : "unresolved");
    std::fprintf(f, "input_source=%s\n", selection.input_source_label ? selection.input_source_label : "unresolved");
    std::fprintf(f, "trusted_source_present=%d\n", selection.trusted_source_present ? 1 : 0);
    std::fprintf(f, "trusted_source_kind=%s\n", selection.trusted_source_kind ? selection.trusted_source_kind : "trusted_profile");
    std::fprintf(f, "trusted_source_runtime_match=%d\n", selection.trusted_source_runtime_match ? 1 : 0);
    std::fprintf(f, "trusted_source_build_match=%d\n", selection.trusted_source_build_match ? 1 : 0);
    std::fprintf(f, "trusted_source_addr=0x%p\n", (void*)selection.trusted_source_addr);
    std::fprintf(f, "trusted_source_accept=%d\n", selection.trusted_source_accept ? 1 : 0);
    std::fclose(f);
}

static void f4mp_prepare_wire_config(const F4MPManualPivotSelection& manual_pivot,
                                     Phase1RRealLocalHookWiringConfig* out_cfg,
                                     bool emit_validation_sample,
                                     bool build_movement_message)
{
    Phase1RRealLocalHookWiringConfig cfg{};
    cfg.require_wire_ready_after_activation = false;
    cfg.emit_validation_sample = emit_validation_sample;
    cfg.build_movement_message = build_movement_message;

    cfg.candidate_config.install_config.expected_local_player_id = 1;
    cfg.candidate_config.install_config.require_hook_correct = true;
    cfg.candidate_config.install_config.patch_allowed = false;
    cfg.candidate_config.install_config.require_update_address =
        manual_pivot.site ? manual_pivot.site->require_update_address : false;
    cfg.candidate_config.install_config.require_input_address =
        manual_pivot.site ? manual_pivot.site->require_input_address : false;
    cfg.candidate_config.install_config.require_callback_after_install = true;
    cfg.candidate_config.install_config.patch_policy = clf4_phpp_default_dry_run();
    cfg.candidate_config.install_config.site_prototype = manual_pivot.site;
    cfg.candidate_config.install_config.site_candidate =
        clf4_phdsp_descriptor(cfg.candidate_config.install_config.site_prototype);
    cfg.candidate_config.callback_source_symbol = manual_pivot.callback_source_symbol;
    cfg.candidate_config.callback_guard_symbol = manual_pivot.callback_guard_symbol;
    cfg.candidate_config.require_supported_runtime_profile = true;

    cfg.callback_validation_config.expected_local_player_id = 1;
    cfg.callback_validation_config.min_callback_forward_count = 1;
    cfg.callback_validation_config.min_callback_accept_count = 1;
    cfg.callback_validation_config.require_runtime_profile_supported = true;
    cfg.callback_validation_config.require_callback_registered = true;
    cfg.callback_validation_config.require_provider_ready = true;
    cfg.callback_validation_config.require_captured_state = true;
    cfg.callback_validation_config.require_expected_player_id_match = true;

    cfg.validation_sample.event.player_id = 1;
    cfg.validation_sample.event.position.x = 21.62f;
    cfg.validation_sample.event.position.y = 1.0f;
    cfg.validation_sample.event.rotation.yaw = 90.0f;
    cfg.validation_sample.event.velocity.x = 6.0f;
    cfg.validation_sample.event.stance = STANCE_RUN;

    if (out_cfg) {
        *out_cfg = cfg;
    }
}

static void f4mp_collect_runtime_traces()
{
    Phase1RRealHookArmingGateConfig gate_cfg{};
    gate_cfg.require_candidate_activated = true;
    gate_cfg.require_runtime_profile_supported = true;
    gate_cfg.require_patch_policy_armed = true;
    (void)phase1r_rhag_evaluate(&gate_cfg);
    Phase1RRealLocalHookWiringState wiring = phase1r_rlhw_state();
    f4mp_write_wiring_trace(&wiring);
}

static void f4mp_write_load_trace(bool query_ok, bool load_ok, UInt32 runtime_version, UInt32 is_editor)
{
    const char* path =
        "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Fallout 4 377160\\Data\\F4SE\\Plugins\\f4mp_load_trace.txt";

    FILE* f = nullptr;
    fopen_s(&f, path, "w");
    if (!f) {
        return;
    }

    std::fprintf(f, "query_ok=%d\n", query_ok ? 1 : 0);
    std::fprintf(f, "load_ok=%d\n", load_ok ? 1 : 0);
    std::fprintf(f, "runtime_version=0x%08X\n", runtime_version);
    std::fprintf(f, "is_editor=%u\n", is_editor);
    std::fclose(f);
}

static void f4mp_write_wiring_trace(const Phase1RRealLocalHookWiringState* s)
{
    const char* path =
        "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Fallout 4 377160\\Data\\F4SE\\Plugins\\f4mp_wiring_trace.txt";

    FILE* f = nullptr;
    fopen_s(&f, path, "w");
    if (!f || !s) {
        return;
    }

    std::fprintf(f, "attempted=%d\n", s->attempted ? 1 : 0);
    std::fprintf(f, "activated=%d\n", s->activated ? 1 : 0);
    std::fprintf(f, "emitted_validation_sample=%d\n", s->emitted_validation_sample ? 1 : 0);
    std::fprintf(f, "callback_validation_passed=%d\n", s->callback_validation_passed ? 1 : 0);
    std::fprintf(f, "wire_ready_after_activation=%d\n", s->wire_ready_after_activation ? 1 : 0);
    std::fprintf(f, "movement_message_built=%d\n", s->movement_message_built ? 1 : 0);
    std::fprintf(f, "wired=%d\n", s->wired ? 1 : 0);
    std::fprintf(f, "runtime_profile_name=%s\n", s->runtime_profile_name ? s->runtime_profile_name : "");
    std::fprintf(f, "site_prototype_name=%s\n", s->site_prototype_name ? s->site_prototype_name : "");

    std::fprintf(f, "candidate.configured=%d\n", s->candidate_state.configured ? 1 : 0);
    std::fprintf(f, "candidate.activated=%d\n", s->candidate_state.activated ? 1 : 0);
    std::fprintf(f, "candidate.install_ready=%d\n", s->candidate_state.install_ready ? 1 : 0);
    std::fprintf(f, "candidate.runtime_profile_supported=%d\n", s->candidate_state.runtime_profile_supported ? 1 : 0);
    std::fprintf(f, "candidate.provider_ready=%d\n", s->candidate_state.provider_ready ? 1 : 0);
    std::fprintf(f, "candidate.callback_registered=%d\n", s->candidate_state.callback_registered ? 1 : 0);
    std::fprintf(f, "candidate.callback_forward_count=%u\n", s->candidate_state.callback_forward_count);
    std::fprintf(f, "candidate.callback_accept_count=%u\n", s->candidate_state.callback_accept_count);
    std::fprintf(f, "candidate.callback_reject_count=%u\n", s->candidate_state.callback_reject_count);
    std::fprintf(f, "candidate.real_callback_forward_count=%u\n", s->candidate_state.real_callback_forward_count);
    std::fprintf(f, "candidate.real_callback_accept_count=%u\n", s->candidate_state.real_callback_accept_count);
    std::fprintf(f, "candidate.real_callback_reject_count=%u\n", s->candidate_state.real_callback_reject_count);
    std::fprintf(f, "candidate.has_state=%d\n", s->candidate_state.has_state ? 1 : 0);
    std::fprintf(f, "candidate.discovery_selected_id=%u\n", s->candidate_state.discovery_selected_candidate_id);
    std::fprintf(f, "candidate.discovery_selected_name=%s\n", s->candidate_state.discovery_selected_candidate_name ? s->candidate_state.discovery_selected_candidate_name : "");
    std::fprintf(f, "candidate.discovery_source=%d\n", (int)s->candidate_state.discovery_selected_address_source);
    std::fprintf(f, "candidate.discovery_update=0x%p\n", (void*)s->candidate_state.discovery_selected_update_addr);
    std::fprintf(f, "candidate.discovery_input=0x%p\n", (void*)s->candidate_state.discovery_selected_input_addr);
    std::fprintf(f, "candidate.effective_patch_symbol=%s\n", s->candidate_state.effective_patch_target_symbol ? s->candidate_state.effective_patch_target_symbol : "");
    std::fprintf(f, "candidate.effective_patch_target=0x%p\n", (void*)s->candidate_state.effective_patch_target_addr);
    std::fprintf(f, "candidate.effective_patch_uses_input=%d\n", s->candidate_state.effective_patch_target_uses_input ? 1 : 0);
    std::fprintf(f, "candidate.planned_detour_entry=%s\n", s->candidate_state.planned_detour_entry_label ? s->candidate_state.planned_detour_entry_label : "");
    std::fprintf(f, "candidate.planned_detour_entry_addr=0x%p\n", (void*)s->candidate_state.planned_detour_entry_addr);
    std::fprintf(f, "candidate.planned_bridge_entry=%s\n", s->candidate_state.planned_bridge_entry_label ? s->candidate_state.planned_bridge_entry_label : "");
    std::fprintf(f, "candidate.planned_bridge_entry_addr=0x%p\n", (void*)s->candidate_state.planned_bridge_entry_addr);
    std::fprintf(f, "candidate.detour_destination=%s\n", s->candidate_state.detour_destination_label ? s->candidate_state.detour_destination_label : "");
    std::fprintf(f, "candidate.detour_destination_addr=0x%p\n", (void*)s->candidate_state.detour_destination_addr);
    std::fprintf(f, "candidate.patch_write_realized=%d\n", s->candidate_state.patch_write_realized ? 1 : 0);
    std::fprintf(f, "candidate.bytes_overwritten=%u\n", s->candidate_state.bytes_overwritten);
    std::fprintf(f, "candidate.trampoline_realized=%d\n", s->candidate_state.trampoline_realized ? 1 : 0);
    std::fprintf(f, "candidate.trampoline_addr=0x%p\n", (void*)s->candidate_state.trampoline_addr);
    std::fprintf(f, "candidate.patch_realization_error=%u\n", s->candidate_state.patch_realization_error_code);
    std::fprintf(f, "candidate.patch_realization_error_label=%s\n", s->candidate_state.patch_realization_error_label ? s->candidate_state.patch_realization_error_label : "");
    std::fprintf(f, "candidate.patch_realization_platform_error=%u\n", s->candidate_state.patch_realization_platform_error_code);
    std::fprintf(f, "candidate.patch_gateway_marker_count=%u\n", s->candidate_state.patch_gateway_marker_count);

    std::fprintf(f, "validation.evaluated=%d\n", s->callback_validation_state.evaluated ? 1 : 0);
    std::fprintf(f, "validation.validated=%d\n", s->callback_validation_state.validated ? 1 : 0);
    std::fprintf(f, "validation.failure=%d\n", (int)s->callback_validation_state.failure);

    std::fprintf(f, "readiness.ready_to_attempt_real_hook=%d\n", s->wire_readiness_state.ready_to_attempt_real_hook ? 1 : 0);
    std::fprintf(f, "readiness.recommended_action=%d\n", (int)s->recommended_action);

    std::fclose(f);
}

static void f4mp_append_live_trace(const char* event_name, int rc, int sent, int applied)
{
    const char* path =
        "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Fallout 4 377160\\Data\\F4SE\\Plugins\\f4mp_live_trace.txt";

    FILE* f = nullptr;
    fopen_s(&f, path, "a");
    if (!f) {
        return;
    }

    std::fprintf(f, "event=%s rc=%d sent=%d applied=%d bridge_initialized=%d\n",
                 event_name ? event_name : "unknown",
                 rc,
                 sent,
                 applied,
                 g_bridge_initialized ? 1 : 0);
    std::fclose(f);
}
}

extern "C" {

__declspec(dllexport) F4SEPluginVersionData F4SEPlugin_Version = {
    F4SEPluginVersionData::kVersion,
    1,
    "f4mp_f4se_plugin",
    "C",
    0,
    0,
    { RUNTIME_VERSION_1_10_984, RUNTIME_VERSION_1_11_191, 0 },
    0,
    0,
    0,
    { 0 }
};

__declspec(dllexport) bool F4SEPlugin_Query(const F4SEInterface* f4se, PluginInfo* info)
{
    clf4_entrypoint_init();

    if (!f4se || !info) {
        return false;
    }

    info->infoVersion = PluginInfo::kInfoVersion;
    info->name = "f4mp_f4se_plugin";
    info->version = 1;

    if (!f4se->isEditor) {
        F4SEInterfaceMock mock = ToMock(f4se);
        const F4MPManualPivotSelection manual_pivot = f4mp_make_manual_pivot_selection(f4se->runtimeVersion);
        Phase1RRealLocalHookWiringConfig wire_cfg{};

        f4mp_bridge_init_once();
        f4mp_write_manual_pivot_trace(manual_pivot, f4se->runtimeVersion);
        f4mp_write_resolution_trace(manual_pivot, f4se->runtimeVersion);
        f4mp_prepare_wire_config(manual_pivot, &wire_cfg, false, false);
        g_query_prearm_active = phase1r_rlhw_attempt(&mock, &manual_pivot.provider, &wire_cfg);
        g_query_prearm_attempted = true;
    }

    return true;
}

__declspec(dllexport) bool F4SEPlugin_Load(const F4SEInterface* f4se)
{
    clf4_entrypoint_init();

    if (!f4se) {
        return false;
    }

    f4mp_bridge_init_once();

    F4SEInterfaceMock mock = ToMock(f4se);
    PluginInfoMock legacyInfo{};
    bool query_ok = clf4_plugin_query(&mock, &legacyInfo);
    bool load_ok = clf4_plugin_load_for_mode(&mock, HOOK_BRINGUP_VANILLA_MIRROR);

    f4mp_write_load_trace(query_ok, load_ok, f4se->runtimeVersion, f4se->isEditor);

    const F4MPManualPivotSelection manual_pivot =
        f4mp_make_manual_pivot_selection(f4se->runtimeVersion);
    f4mp_write_manual_pivot_trace(manual_pivot, f4se->runtimeVersion);
    f4mp_write_resolution_trace(manual_pivot, f4se->runtimeVersion);

    if (!g_query_prearm_attempted || !phase1r_rlhw_state().candidate_state.install_ready) {
        Phase1RRealLocalHookWiringConfig wire_cfg{};
        f4mp_prepare_wire_config(manual_pivot, &wire_cfg, false, false);
        (void)phase1r_rlhw_attempt(&mock, &manual_pivot.provider, &wire_cfg);
        g_query_prearm_active = phase1r_rlhw_state().candidate_state.install_ready;
        g_query_prearm_attempted = true;
    }

    f4mp_collect_runtime_traces();
    return true;
}

F4MP_PLUGIN_EXPORT bool F4MP_LiveTick(bool* out_sent, bool* out_applied)
{
    bool sent = false;
    bool applied = false;
    bool rc;

    f4mp_bridge_init_once();
    rc = ebstub_live_tick(&sent, &applied);

    if (out_sent) {
        *out_sent = sent;
    }
    if (out_applied) {
        *out_applied = applied;
    }

    f4mp_append_live_trace("F4MP_LiveTick", rc ? 1 : 0, sent ? 1 : 0, applied ? 1 : 0);
    return rc;
}

F4MP_PLUGIN_EXPORT bool F4MP_LiveEnabled(void)
{
    bool rc;
    f4mp_bridge_init_once();
    rc = ebstub_live_enabled();
    f4mp_append_live_trace("F4MP_LiveEnabled", rc ? 1 : 0, 0, 0);
    return rc;
}

F4MP_PLUGIN_EXPORT bool F4MP_LiveConnected(void)
{
    bool rc;
    f4mp_bridge_init_once();
    rc = ebstub_live_connected();
    f4mp_append_live_trace("F4MP_LiveConnected", rc ? 1 : 0, 0, 0);
    return rc;
}

F4MP_PLUGIN_EXPORT const char* F4MP_LiveConfigPath(void)
{
    const char* path;
    f4mp_bridge_init_once();
    path = ebstub_live_config_path();
    f4mp_append_live_trace("F4MP_LiveConfigPath", path && path[0] ? 1 : 0, 0, 0);
    return path;
}

F4MP_PLUGIN_EXPORT void F4MP_Shutdown(void)
{
    f4mp_append_live_trace("F4MP_Shutdown", 1, 0, 0);
    f4mp_bridge_shutdown_once();
}

}

#ifdef _WIN32
BOOL APIENTRY DllMain(HMODULE, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_DETACH) {
        f4mp_bridge_shutdown_once();
    }
    return TRUE;
}
#endif
