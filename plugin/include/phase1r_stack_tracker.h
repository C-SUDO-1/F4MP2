#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum Phase1RStackStatus {
    PHASE1R_STACK_DONE = 0,
    PHASE1R_STACK_CURRENT = 1,
    PHASE1R_STACK_NEXT = 2,
    PHASE1R_STACK_BLOCKED = 3
} Phase1RStackStatus;

typedef struct Phase1RStackEntry {
    const char* name;
    Phase1RStackStatus status;
} Phase1RStackEntry;

size_t phase1r_stack_tracker_snapshot(Phase1RStackEntry* out_entries, size_t capacity);
const char* phase1r_stack_status_name(Phase1RStackStatus status);

#ifdef __cplusplus
}
#endif
