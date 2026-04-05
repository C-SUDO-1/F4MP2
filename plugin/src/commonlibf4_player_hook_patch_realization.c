#include "commonlibf4_player_hook_patch_realization.h"
#include "player_hook_shim.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#endif

static CommonLibF4PlayerHookPatchRealizationState g_state;
#ifdef _WIN32
static volatile LONG g_marker_gateway_count = 0;
#endif
static const char* VERIFY_TRACE_PATH = "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Fallout 4 377160\\Data\\F4SE\\Plugins\\f4mp_patch_plan_verify_trace.txt";

static void emit_mov_rax_jmp_rax(unsigned char* dst, uintptr_t addr);

static bool is_supported_detour_label(const char* label) {
    if (!label || !label[0]) {
        return false;
    }
    if (strcmp(label, "phs_create_player_controls_manual_detour_thunk") == 0) {
        return true;
    }
    if (strcmp(label, "phpr_passthrough_gateway") == 0) {
        return true;
    }
    if (strcmp(label, "phpr_marker_gateway") == 0) {
        return true;
    }
    if (strcmp(label, "phs_note_manual_detour_entry") == 0) {
        return false;
    }
    if (strcmp(label, "phs_ingest_real_player_snapshot") == 0) {
        return false;
    }
    return false;
}


static bool use_marker_gateway_variant(void) {
    char value[32];
    DWORD len = 0u;
    memset(value, 0, sizeof(value));
    len = GetEnvironmentVariableA("F4MP_GATEWAY_VARIANT", value, (DWORD)sizeof(value));
    if (len == 0u || len >= (DWORD)sizeof(value)) {
        /* Slice 4: default to the marker gateway so a real post-install hit leaves
           hard evidence even when the site cannot yet build a real movement sample. */
        return true;
    }
    if (_stricmp(value, "pass") == 0 || _stricmp(value, "passthrough") == 0) {
        return false;
    }
    if (_stricmp(value, "marker") == 0 || _stricmp(value, "mark") == 0) {
        return true;
    }
    return true;
}

static unsigned char* allocate_passthrough_gateway(uintptr_t trampoline_addr) {
    enum { PASSTHROUGH_GATEWAY_BYTES = 32, DETOUR_STUB_BYTES = 12 };
    unsigned char* gateway = NULL;
    size_t cursor = 0u;
    if (trampoline_addr == 0u) {
        return NULL;
    }
    gateway = (unsigned char*)VirtualAlloc(NULL, (SIZE_T)PASSTHROUGH_GATEWAY_BYTES, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!gateway) {
        return NULL;
    }
    emit_mov_rax_jmp_rax(gateway + cursor, trampoline_addr);
    cursor += DETOUR_STUB_BYTES;
    while (cursor < PASSTHROUGH_GATEWAY_BYTES) {
        gateway[cursor++] = 0x90u;
    }
    FlushInstructionCache(GetCurrentProcess(), gateway, (SIZE_T)PASSTHROUGH_GATEWAY_BYTES);
    return gateway;
}

const char* clf4_phpr_error_label(CommonLibF4PlayerHookPatchRealizationError error) {
    switch (error) {
        case CLF4_PATCH_REALIZATION_ERR_NONE: return "none";
        case CLF4_PATCH_REALIZATION_ERR_NOT_ATTEMPTED: return "not_attempted";
        case CLF4_PATCH_REALIZATION_ERR_TARGET_MISSING: return "target_missing";
        case CLF4_PATCH_REALIZATION_ERR_DETOUR_MISSING: return "detour_missing";
        case CLF4_PATCH_REALIZATION_ERR_INCOMPATIBLE_DETOUR_ENTRY: return "incompatible_detour_entry";
        case CLF4_PATCH_REALIZATION_ERR_PROTECT_FAILED: return "protect_failed";
        case CLF4_PATCH_REALIZATION_ERR_TRAMPOLINE_ALLOC_FAILED: return "trampoline_alloc_failed";
        case CLF4_PATCH_REALIZATION_ERR_WRITE_FAILED: return "write_failed";
        case CLF4_PATCH_REALIZATION_ERR_UNSUPPORTED_PLATFORM: return "unsupported_platform";
        case CLF4_PATCH_REALIZATION_ERR_VERIFICATION_ONLY_SAFETY_ROLLBACK: return "verification_only_safety_rollback";
        case CLF4_PATCH_REALIZATION_ERR_PLANNER_UNSUPPORTED_INSTRUCTION: return "planner_unsupported_instruction";
        default: return "unknown";
    }
}

static void set_error(CommonLibF4PlayerHookPatchRealizationError error, uint32_t platform_error_code) {
    g_state.error = error;
    g_state.error_label = clf4_phpr_error_label(error);
    g_state.platform_error_code = platform_error_code;
}

void clf4_phpr_reset(void) {
    memset(&g_state, 0, sizeof(g_state));
    g_state.error = CLF4_PATCH_REALIZATION_ERR_NOT_ATTEMPTED;
    g_state.error_label = clf4_phpr_error_label(g_state.error);
#ifdef _WIN32
    g_marker_gateway_count = 0;
#endif
    phs_set_manual_create_player_controls_trampoline(0u);
}

CommonLibF4PlayerHookPatchRealizationState clf4_phpr_state(void) {
#ifdef _WIN32
    g_state.gateway_marker_count = (uint32_t)g_marker_gateway_count;
#endif
    return g_state;
}

#ifdef _WIN32
typedef struct PatchPlanInstructionInfo {
    size_t len;
    bool reloc_safe;
} PatchPlanInstructionInfo;

typedef struct PatchPlanBoundaryResult {
    bool decode_supported;
    bool relocation_safe;
    size_t instruction_count;
    size_t overwrite_len;
    size_t min_jump_bytes;
    unsigned char lengths[8];
} PatchPlanBoundaryResult;

static bool parse_modrm_sib(const unsigned char* code, size_t avail, size_t* out_len, bool* out_rip_relative) {
    unsigned char modrm;
    unsigned char mod;
    unsigned char rm;
    size_t cursor = 0u;
    if (!code || avail < 1u || !out_len || !out_rip_relative) {
        return false;
    }
    modrm = code[cursor++];
    mod = (unsigned char)((modrm >> 6) & 0x3u);
    rm = (unsigned char)(modrm & 0x7u);
    *out_rip_relative = (mod == 0u && rm == 5u);
    if (rm == 4u && mod != 3u) {
        unsigned char sib;
        unsigned char base;
        if (cursor >= avail) {
            return false;
        }
        sib = code[cursor++];
        base = (unsigned char)(sib & 0x7u);
        if (mod == 0u && base == 5u) {
            if ((cursor + 4u) > avail) {
                return false;
            }
            cursor += 4u;
        }
    }
    if (mod == 1u) {
        if ((cursor + 1u) > avail) {
            return false;
        }
        cursor += 1u;
    } else if (mod == 2u || (mod == 0u && rm == 5u)) {
        if ((cursor + 4u) > avail) {
            return false;
        }
        cursor += 4u;
    }
    *out_len = cursor;
    return true;
}

static bool decode_instruction_x64(const unsigned char* code, size_t avail, PatchPlanInstructionInfo* out) {
    size_t cursor = 0u;
    bool reloc_safe = true;
    unsigned char opcode = 0u;
    if (!code || !out || avail == 0u) {
        return false;
    }
    while (cursor < avail) {
        unsigned char b = code[cursor];
        if ((b >= 0x40u && b <= 0x4Fu) || b == 0x66u || b == 0x67u || b == 0xF2u || b == 0xF3u) {
            cursor += 1u;
            continue;
        }
        break;
    }
    if (cursor >= avail) {
        return false;
    }
    opcode = code[cursor++];
    if ((opcode >= 0x50u && opcode <= 0x5Fu) || opcode == 0x90u || opcode == 0xC3u || opcode == 0xCCu) {
        out->len = cursor;
        out->reloc_safe = reloc_safe;
        return true;
    }
    if (opcode == 0x68u) {
        if ((cursor + 4u) > avail) return false;
        out->len = cursor + 4u;
        out->reloc_safe = reloc_safe;
        return true;
    }
    if (opcode == 0x6Au) {
        if ((cursor + 1u) > avail) return false;
        out->len = cursor + 1u;
        out->reloc_safe = reloc_safe;
        return true;
    }
    if (opcode == 0xE8u || opcode == 0xE9u) {
        if ((cursor + 4u) > avail) return false;
        out->len = cursor + 4u;
        out->reloc_safe = false;
        return true;
    }
    if (opcode == 0xEBu || (opcode >= 0x70u && opcode <= 0x7Fu)) {
        if ((cursor + 1u) > avail) return false;
        out->len = cursor + 1u;
        out->reloc_safe = false;
        return true;
    }
    if (opcode == 0x0Fu) {
        unsigned char op2;
        size_t extra = 0u;
        bool rip_relative = false;
        if (cursor >= avail) return false;
        op2 = code[cursor++];
        if (op2 >= 0x80u && op2 <= 0x8Fu) {
            if ((cursor + 4u) > avail) return false;
            out->len = cursor + 4u;
            out->reloc_safe = false;
            return true;
        }
        if (op2 == 0x1Fu) {
            if (!parse_modrm_sib(code + cursor, avail - cursor, &extra, &rip_relative)) return false;
            out->len = cursor + extra;
            out->reloc_safe = reloc_safe && !rip_relative;
            return true;
        }
        return false;
    }
    if (opcode >= 0xB8u && opcode <= 0xBFu) {
        size_t imm_len = 4u;
        bool rex_w = false;
        size_t i;
        for (i = 0u; i < cursor; ++i) {
            unsigned char p = code[i];
            if (p >= 0x40u && p <= 0x4Fu && (p & 0x08u)) {
                rex_w = true;
            }
        }
        if (rex_w) imm_len = 8u;
        if ((cursor + imm_len) > avail) return false;
        out->len = cursor + imm_len;
        out->reloc_safe = reloc_safe;
        return true;
    }
    {
        size_t modrm_extra = 0u;
        bool rip_relative = false;
        switch (opcode) {
            case 0x01u: case 0x03u: case 0x21u: case 0x23u: case 0x29u: case 0x2Bu:
            case 0x31u: case 0x33u: case 0x39u: case 0x3Bu: case 0x63u: case 0x85u:
            case 0x87u: case 0x89u: case 0x8Bu: case 0x8Du: case 0xFFu:
                if (!parse_modrm_sib(code + cursor, avail - cursor, &modrm_extra, &rip_relative)) return false;
                out->len = cursor + modrm_extra;
                out->reloc_safe = reloc_safe && !rip_relative;
                return true;
            case 0x80u: case 0x82u: case 0x83u: case 0xC0u: case 0xC1u: case 0xC6u:
                if (!parse_modrm_sib(code + cursor, avail - cursor, &modrm_extra, &rip_relative)) return false;
                if ((cursor + modrm_extra + 1u) > avail) return false;
                out->len = cursor + modrm_extra + 1u;
                out->reloc_safe = reloc_safe && !rip_relative;
                return true;
            case 0x81u: case 0x69u: case 0xC7u:
                if (!parse_modrm_sib(code + cursor, avail - cursor, &modrm_extra, &rip_relative)) return false;
                if ((cursor + modrm_extra + 4u) > avail) return false;
                out->len = cursor + modrm_extra + 4u;
                out->reloc_safe = reloc_safe && !rip_relative;
                return true;
            default:
                return false;
        }
    }
}

static PatchPlanBoundaryResult compute_patch_plan_boundary(const unsigned char* bytes, size_t count, size_t min_jump_bytes) {
    PatchPlanBoundaryResult out;
    size_t cursor = 0u;
    memset(&out, 0, sizeof(out));
    out.decode_supported = true;
    out.relocation_safe = true;
    out.min_jump_bytes = min_jump_bytes;
    while (cursor < count && out.instruction_count < (sizeof(out.lengths) / sizeof(out.lengths[0])) && cursor < min_jump_bytes) {
        PatchPlanInstructionInfo info;
        if (!decode_instruction_x64(bytes + cursor, count - cursor, &info) || info.len == 0u) {
            out.decode_supported = false;
            out.relocation_safe = false;
            return out;
        }
        out.lengths[out.instruction_count++] = (unsigned char)info.len;
        if (!info.reloc_safe) {
            out.relocation_safe = false;
        }
        cursor += info.len;
    }
    if (cursor < min_jump_bytes) {
        out.decode_supported = false;
        out.relocation_safe = false;
        return out;
    }
    out.overwrite_len = cursor;
    return out;
}

static void bytes_to_hex(const unsigned char* bytes, size_t count, char* out, size_t out_cap) {
    static const char* HEX = "0123456789ABCDEF";
    size_t i = 0u;
    size_t cursor = 0u;
    if (!out || out_cap == 0u) {
        return;
    }
    out[0] = '\0';
    if (!bytes) {
        return;
    }
    for (i = 0u; i < count && (cursor + 3u) < out_cap; ++i) {
        out[cursor++] = HEX[(bytes[i] >> 4) & 0xF];
        out[cursor++] = HEX[bytes[i] & 0xF];
        if (i + 1u < count && (cursor + 1u) < out_cap) {
            out[cursor++] = ' ';
        }
    }
    out[cursor] = '\0';
}

static void write_verify_trace(const CommonLibF4PlayerHookPatchRealizationPlan* plan,
                               const MEMORY_BASIC_INFORMATION* mbi,
                               const unsigned char* bytes,
                               size_t byte_count,
                               const PatchPlanBoundaryResult* planner) {
    FILE* f = NULL;
    char hexbuf[16u * 3u + 1u];
    char lengthsbuf[64u];
    size_t i = 0u;
    size_t cursor = 0u;
    const uintptr_t target = plan ? plan->target_addr : 0u;
    const uintptr_t detour = plan ? plan->detour_destination_addr : 0u;
    bytes_to_hex(bytes, byte_count, hexbuf, sizeof(hexbuf));
    lengthsbuf[0] = '\0';
    if (planner) {
        for (i = 0u; i < planner->instruction_count; ++i) {
            int wrote = snprintf(lengthsbuf + cursor, sizeof(lengthsbuf) - cursor,
                                 "%s%u", (i == 0u) ? "" : ",", (unsigned int)planner->lengths[i]);
            if (wrote <= 0) {
                break;
            }
            cursor += (size_t)wrote;
            if (cursor >= sizeof(lengthsbuf)) {
                cursor = sizeof(lengthsbuf) - 1u;
                break;
            }
        }
    }
    fopen_s(&f, VERIFY_TRACE_PATH, "a");
    if (!f) {
        return;
    }
    fprintf(f, "event=verify_only_plan\n");
    fprintf(f, "target_symbol=%s\n", (plan && plan->target_symbol) ? plan->target_symbol : "");
    fprintf(f, "target_addr=0x%p\n", (void*)target);
    fprintf(f, "detour_destination_label=%s\n", (plan && plan->detour_destination_label) ? plan->detour_destination_label : "");
    fprintf(f, "detour_destination_addr=0x%p\n", (void*)detour);
    fprintf(f, "planned_min_jump_bytes=%u\n", planner ? (unsigned int)planner->min_jump_bytes : 0u);
    fprintf(f, "planner_decode_supported=%d\n", (planner && planner->decode_supported) ? 1 : 0);
    fprintf(f, "planner_instruction_count=%u\n", planner ? (unsigned int)planner->instruction_count : 0u);
    fprintf(f, "planner_instruction_lengths=%s\n", lengthsbuf);
    fprintf(f, "planner_boundary_overwrite_bytes=%u\n", planner ? (unsigned int)planner->overwrite_len : 0u);
    fprintf(f, "planner_relocation_safe=%d\n", (planner && planner->relocation_safe) ? 1 : 0);
    fprintf(f, "captured_bytes_count=%u\n", (unsigned int)byte_count);
    fprintf(f, "captured_bytes_hex=%s\n", hexbuf);
    fprintf(f, "page_base=0x%p\n", mbi ? mbi->BaseAddress : NULL);
    fprintf(f, "page_region_size=0x%p\n", mbi ? (void*)mbi->RegionSize : NULL);
    fprintf(f, "page_state=0x%08X\n", mbi ? (unsigned int)mbi->State : 0u);
    fprintf(f, "page_protect=0x%08X\n", mbi ? (unsigned int)mbi->Protect : 0u);
    fprintf(f, "page_type=0x%08X\n", mbi ? (unsigned int)mbi->Type : 0u);
    fprintf(f, "result=%s\n", clf4_phpr_error_label(g_state.error));
    fclose(f);
}
#endif

static void emit_mov_rax_jmp_rax(unsigned char* dst, uintptr_t addr) {
    uint64_t u = (uint64_t)addr;
    if (!dst) {
        return;
    }
    dst[0] = 0x48u;
    dst[1] = 0xB8u;
    memcpy(dst + 2u, &u, sizeof(u));
    dst[10] = 0xFFu;
    dst[11] = 0xE0u;
}


#ifdef _WIN32
static unsigned char* allocate_marker_gateway(uintptr_t trampoline_addr) {
    enum { MARKER_GATEWAY_BYTES = 64, DETOUR_STUB_BYTES = 12 };
    unsigned char* gateway = NULL;
    uint64_t counter_addr = 0u;
    size_t cursor = 0u;
    if (trampoline_addr == 0u) {
        return NULL;
    }
    gateway = (unsigned char*)VirtualAlloc(NULL, (SIZE_T)MARKER_GATEWAY_BYTES, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!gateway) {
        return NULL;
    }
    counter_addr = (uint64_t)(uintptr_t)&g_marker_gateway_count;
    gateway[cursor++] = 0x9Cu; /* pushfq */
    gateway[cursor++] = 0x50u; /* push rax */
    gateway[cursor++] = 0x41u; gateway[cursor++] = 0x53u; /* push r11 */
    gateway[cursor++] = 0x49u; gateway[cursor++] = 0xBBu; /* mov r11, imm64 */
    memcpy(gateway + cursor, &counter_addr, sizeof(counter_addr));
    cursor += sizeof(counter_addr);
    gateway[cursor++] = 0x41u; gateway[cursor++] = 0x83u; gateway[cursor++] = 0x03u; gateway[cursor++] = 0x01u; /* add dword ptr [r11], 1 */
    gateway[cursor++] = 0x41u; gateway[cursor++] = 0x5Bu; /* pop r11 */
    gateway[cursor++] = 0x58u; /* pop rax */
    gateway[cursor++] = 0x9Du; /* popfq */
    emit_mov_rax_jmp_rax(gateway + cursor, trampoline_addr);
    cursor += DETOUR_STUB_BYTES;
    while (cursor < MARKER_GATEWAY_BYTES) {
        gateway[cursor++] = 0x90u;
    }
    FlushInstructionCache(GetCurrentProcess(), gateway, (SIZE_T)MARKER_GATEWAY_BYTES);
    return gateway;
}
#endif

bool clf4_phpr_attempt(const CommonLibF4PlayerHookPatchRealizationPlan* plan) {
    clf4_phpr_reset();
    g_state.attempted = true;
    g_state.target_addr = plan ? plan->target_addr : 0u;
    g_state.target_symbol = plan ? plan->target_symbol : NULL;
    g_state.detour_destination_addr = plan ? plan->detour_destination_addr : 0u;
    g_state.detour_destination_label = plan ? plan->detour_destination_label : NULL;

    if (!plan || plan->target_addr == 0u) {
        set_error(CLF4_PATCH_REALIZATION_ERR_TARGET_MISSING, 0u);
        return false;
    }
    if (plan->detour_destination_addr == 0u) {
        set_error(CLF4_PATCH_REALIZATION_ERR_DETOUR_MISSING, 0u);
        return false;
    }
    if (!is_supported_detour_label(plan->detour_destination_label)) {
        set_error(CLF4_PATCH_REALIZATION_ERR_INCOMPATIBLE_DETOUR_ENTRY, 0u);
        return false;
    }

#ifndef _WIN32
    set_error(CLF4_PATCH_REALIZATION_ERR_UNSUPPORTED_PLATFORM, 0u);
    return false;
#else
    {
        enum { VERIFY_CAPTURE_BYTES = 16, VERIFY_MIN_JUMP_BYTES = 12, DETOUR_STUB_BYTES = 12, TRAMPOLINE_TAIL_BYTES = 12 };
        MEMORY_BASIC_INFORMATION mbi;
        unsigned char captured[VERIFY_CAPTURE_BYTES];
        PatchPlanBoundaryResult planner;
        SIZE_T q = 0u;
        DWORD oldProtect = 0u;
        DWORD restoreProtect = 0u;
        unsigned char* trampoline = NULL;
        SIZE_T trampoline_size = 0u;
        size_t overwrite_len = 0u;
        memset(&mbi, 0, sizeof(mbi));
        memset(captured, 0, sizeof(captured));
        memcpy(captured, (const void*)plan->target_addr, sizeof(captured));
        q = VirtualQuery((LPCVOID)plan->target_addr, &mbi, sizeof(mbi));
        planner = compute_patch_plan_boundary(captured, sizeof(captured), VERIFY_MIN_JUMP_BYTES);
        write_verify_trace(plan, q == sizeof(mbi) ? &mbi : NULL, captured, sizeof(captured), &planner);
        if (!planner.decode_supported || !planner.relocation_safe) {
            set_error(CLF4_PATCH_REALIZATION_ERR_PLANNER_UNSUPPORTED_INSTRUCTION, 0u);
            return false;
        }
        overwrite_len = planner.overwrite_len;
        if (overwrite_len < VERIFY_MIN_JUMP_BYTES) {
            set_error(CLF4_PATCH_REALIZATION_ERR_WRITE_FAILED, 0u);
            return false;
        }

        unsigned char* detour_gateway = NULL;
        uintptr_t realized_detour_destination = plan->detour_destination_addr;
        const char* realized_detour_label = plan->detour_destination_label;
        const bool marker_variant = use_marker_gateway_variant();

        trampoline_size = (SIZE_T)overwrite_len + (SIZE_T)TRAMPOLINE_TAIL_BYTES;
        trampoline = (unsigned char*)VirtualAlloc(NULL, trampoline_size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
        if (!trampoline) {
            set_error(CLF4_PATCH_REALIZATION_ERR_TRAMPOLINE_ALLOC_FAILED, (uint32_t)GetLastError());
            return false;
        }
        memcpy(trampoline, (const void*)plan->target_addr, overwrite_len);
        emit_mov_rax_jmp_rax(trampoline + overwrite_len, (uintptr_t)((const unsigned char*)plan->target_addr + overwrite_len));
        FlushInstructionCache(GetCurrentProcess(), trampoline, trampoline_size);

        if (marker_variant) {
            detour_gateway = allocate_marker_gateway((uintptr_t)trampoline);
            if (!detour_gateway) {
                VirtualFree(trampoline, 0u, MEM_RELEASE);
                set_error(CLF4_PATCH_REALIZATION_ERR_TRAMPOLINE_ALLOC_FAILED, (uint32_t)GetLastError());
                return false;
            }
            realized_detour_destination = (uintptr_t)detour_gateway;
            realized_detour_label = "phpr_marker_gateway";
        } else {
            detour_gateway = allocate_passthrough_gateway((uintptr_t)trampoline);
            if (!detour_gateway) {
                VirtualFree(trampoline, 0u, MEM_RELEASE);
                set_error(CLF4_PATCH_REALIZATION_ERR_TRAMPOLINE_ALLOC_FAILED, (uint32_t)GetLastError());
                return false;
            }
            realized_detour_destination = (uintptr_t)detour_gateway;
            realized_detour_label = "phpr_passthrough_gateway";
        }

        if (!VirtualProtect((LPVOID)plan->target_addr, (SIZE_T)overwrite_len, PAGE_EXECUTE_READWRITE, &oldProtect)) {
            VirtualFree(detour_gateway, 0u, MEM_RELEASE);
            VirtualFree(trampoline, 0u, MEM_RELEASE);
            set_error(CLF4_PATCH_REALIZATION_ERR_PROTECT_FAILED, (uint32_t)GetLastError());
            return false;
        }

        emit_mov_rax_jmp_rax((unsigned char*)(uintptr_t)plan->target_addr, realized_detour_destination);
        if (overwrite_len > DETOUR_STUB_BYTES) {
            memset((void*)((uintptr_t)plan->target_addr + DETOUR_STUB_BYTES), 0x90, overwrite_len - DETOUR_STUB_BYTES);
        }
        FlushInstructionCache(GetCurrentProcess(), (LPCVOID)plan->target_addr, (SIZE_T)overwrite_len);

        if (!VirtualProtect((LPVOID)plan->target_addr, (SIZE_T)overwrite_len, oldProtect, &restoreProtect)) {
            /* keep going; patch has been written */
        }

        g_state.patch_write_realized = true;
        g_state.trampoline_realized = true;
        g_state.trampoline_addr = (uintptr_t)trampoline;
        g_state.bytes_overwritten = (uint32_t)overwrite_len;
        g_state.detour_destination_addr = realized_detour_destination;
        g_state.detour_destination_label = realized_detour_label;
        set_error(CLF4_PATCH_REALIZATION_ERR_NONE, 0u);
        phs_set_manual_create_player_controls_trampoline((uintptr_t)trampoline);
        return true;
    }
#endif
}
