#pragma once

#include <stdbool.h>

bool replay_log_open(const char* path);
void replay_log_close(void);
void replay_log_write(const char* event_type, const char* result, const char* details_json);
