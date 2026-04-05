#pragma once
#include <stdbool.h>
#include <sys/types.h>
typedef struct F4mpServiceProcess { pid_t pid; } F4mpServiceProcess;
bool f4mp_spawn_service(F4mpServiceProcess* proc, const char* port);
void f4mp_stop_service(F4mpServiceProcess* proc);
const char* f4mp_service_binary_path(void);
