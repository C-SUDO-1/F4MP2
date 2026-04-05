#pragma once

#ifndef _MSC_VER
#include <stdio.h>
static inline int f4mp_portable_fopen_s(FILE** out_file, const char* path, const char* mode) {
    if (!out_file) {
        return 1;
    }
    *out_file = fopen(path, mode);
    return (*out_file != NULL) ? 0 : 1;
}
#define fopen_s(out_file, path, mode) f4mp_portable_fopen_s((out_file), (path), (mode))
#endif
