#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef volatile void* coroutine_t;
typedef const void* (*coroutine_entry_point)(coroutine_t, const void*);

#if _WIN32 && _M_IX86
#define CDECL __cdecl
#else
#define CDECL
#endif

coroutine_t CDECL coroutine_new(coroutine_entry_point point, size_t stack_size);
const void* CDECL coroutine_resume(coroutine_t coro, const void* param);
const void* CDECL coroutine_yield(coroutine_t coro, const void* param);
int CDECL is_coroutine_ended(coroutine_t coro);
void CDECL coroutine_destroy(coroutine_t coro);

#ifdef __cplusplus
}
#endif
