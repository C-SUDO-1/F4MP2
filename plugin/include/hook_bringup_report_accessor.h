#pragma once

#include <stdbool.h>
#include "hook_attempt_result.h"

#ifdef __cplusplus
extern "C" {
#endif

void hook_bringup_report_reset(void);
void hook_bringup_report_store(const HookAttemptResult* results, unsigned int count);
bool hook_bringup_report_has_value(void);
unsigned int hook_bringup_report_count(void);
bool hook_bringup_report_get_result(unsigned int index, HookAttemptResult* out_result);

#ifdef __cplusplus
}
#endif
