#pragma once

#include <stdbool.h>
#include "commonlibf4_hook_adapter_stub.h"
#include "commonlibf4_player_hook_dispatch_context.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef bool (*CommonLibF4PlayerHookCallback)(const CommonLibF4PlayerHookArgs* args, void* user);

typedef struct CommonLibF4PlayerHookCallbackRegistration {
    CommonLibF4PlayerHookCallback callback;
    void* user;
} CommonLibF4PlayerHookCallbackRegistration;

typedef struct CommonLibF4PlayerHookCallbackStubStats {
    bool registered_callback;
    unsigned int register_count;
    unsigned int emit_count;
    unsigned int synthetic_emit_count;
    unsigned int real_emit_count;
    unsigned int callback_success_count;
    unsigned int callback_failure_count;
    unsigned int real_callback_success_count;
    unsigned int real_callback_failure_count;
    CommonLibF4PlayerHookDispatchKind last_dispatch_kind;
    bool has_last_args;
    CommonLibF4PlayerHookArgs last_args;
} CommonLibF4PlayerHookCallbackStubStats;

void clf4_player_hook_callback_stub_register(const CommonLibF4PlayerHookCallbackRegistration* reg);
void clf4_player_hook_callback_stub_clear(void);
bool clf4_player_hook_callback_stub_emit(const CommonLibF4PlayerHookArgs* args);
bool clf4_player_hook_callback_real_emit(const CommonLibF4PlayerHookArgs* args);
CommonLibF4PlayerHookCallbackStubStats clf4_player_hook_callback_stub_stats(void);

#ifdef __cplusplus
}
#endif
