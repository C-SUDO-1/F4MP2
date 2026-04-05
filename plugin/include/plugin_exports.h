#pragma once

#if defined(_WIN32) || defined(_WIN64)
  #if defined(F4MP_PLUGIN_BUILD)
    #define F4MP_PLUGIN_API __declspec(dllexport)
  #else
    #define F4MP_PLUGIN_API __declspec(dllimport)
  #endif
#else
  #define F4MP_PLUGIN_API __attribute__((visibility("default")))
#endif

#if defined(__cplusplus)
  #define F4MP_EXTERN_C extern "C"
#else
  #define F4MP_EXTERN_C
#endif

#define F4MP_PLUGIN_EXPORT F4MP_EXTERN_C F4MP_PLUGIN_API
