#pragma once

#include <stdbool.h>
#include "commonlibf4_hook_adapter_stub.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef bool (*CommonLibF4ActorHookCallback)(const CommonLibF4ActorHookArgs* args, void* user);

typedef struct CommonLibF4ActorHookCallbackRegistration {
    CommonLibF4ActorHookCallback callback;
    void* user;
} CommonLibF4ActorHookCallbackRegistration;

typedef struct CommonLibF4ActorHookCallbackStubStats {
    bool registered_callback;
    unsigned int register_count;
    unsigned int emit_count;
    unsigned int callback_success_count;
    unsigned int callback_failure_count;
    bool has_last_args;
    CommonLibF4ActorHookArgs last_args;
} CommonLibF4ActorHookCallbackStubStats;

void clf4_actor_hook_callback_stub_register(const CommonLibF4ActorHookCallbackRegistration* reg);
void clf4_actor_hook_callback_stub_clear(void);
bool clf4_actor_hook_callback_stub_emit(const CommonLibF4ActorHookArgs* args);
CommonLibF4ActorHookCallbackStubStats clf4_actor_hook_callback_stub_stats(void);

#ifdef __cplusplus
}
#endif
