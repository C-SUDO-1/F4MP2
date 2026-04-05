#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "commonlibf4_address_provider.h"
#include "commonlibf4_entrypoint_stub.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CommonLibF4PlayerHookDetourSiteCandidateDescriptor {
    const char* name;
    CommonLibF4AddressKey update_key;
    CommonLibF4AddressKey input_key;
    uint32_t minimum_runtime_version;
    bool require_update_address;
    bool require_input_address;
    bool editor_supported;
} CommonLibF4PlayerHookDetourSiteCandidateDescriptor;

typedef struct CommonLibF4PlayerHookDetourSiteCandidateResolution {
    const CommonLibF4PlayerHookDetourSiteCandidateDescriptor* descriptor;
    bool valid;
    bool runtime_supported;
    bool update_resolved;
    bool input_resolved;
    uintptr_t update_addr;
    uintptr_t input_addr;
} CommonLibF4PlayerHookDetourSiteCandidateResolution;

const CommonLibF4PlayerHookDetourSiteCandidateDescriptor* clf4_phdsc_default(void);

bool clf4_phdsc_resolve(
    const CommonLibF4PlayerHookDetourSiteCandidateDescriptor* descriptor,
    const F4SEInterfaceMock* f4se,
    const CommonLibF4AddressProvider* provider,
    CommonLibF4PlayerHookDetourSiteCandidateResolution* out_resolution
);

#ifdef __cplusplus
}
#endif
