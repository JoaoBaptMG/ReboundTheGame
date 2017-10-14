#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef volatile void* coroutine_t;
typedef const void* (*coroutine_entry_point)(coroutine_t, const void*);

coroutine_t coroutine_new(coroutine_entry_point point, size_t stack_size);
const void* coroutine_resume(coroutine_t coro, const void* param);
const void* coroutine_yield(coroutine_t coro, const void* param);
int is_coroutine_ended(coroutine_t coro);
void coroutine_destroy(coroutine_t coro);

#ifdef __cplusplus
}
#endif
