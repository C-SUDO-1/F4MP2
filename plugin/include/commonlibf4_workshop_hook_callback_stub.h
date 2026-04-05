#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "workshop_event_translation.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef bool (*CommonLibF4WorkshopHookCallback)(const WorkshopHostEvent* ev, void* user);

typedef struct CommonLibF4WorkshopHookCallbackRegistration {
    CommonLibF4WorkshopHookCallback callback;
    void* user;
} CommonLibF4WorkshopHookCallbackRegistration;

typedef struct CommonLibF4WorkshopHookCallbackStubStats {
    bool registered_callback;
    uint32_t register_count;
    uint32_t emit_count;
    uint32_t callback_success_count;
    uint32_t callback_failure_count;
    bool has_last_event;
    WorkshopHostEvent last_event;
} CommonLibF4WorkshopHookCallbackStubStats;

void clf4_workshop_hook_callback_stub_register(const CommonLibF4WorkshopHookCallbackRegistration* reg);
void clf4_workshop_hook_callback_stub_clear(void);
bool clf4_workshop_hook_callback_stub_emit(const WorkshopHostEvent* ev);
CommonLibF4WorkshopHookCallbackStubStats clf4_workshop_hook_callback_stub_stats(void);

#ifdef __cplusplus
}
#endif
