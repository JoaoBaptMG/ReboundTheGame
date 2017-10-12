#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef void* coroutine_t;
typedef void* (*coroutine_entry_point)(coroutine_t, void*);

coroutine_t coroutine_new(coroutine_entry_point point, size_t stack_size);
void* coroutine_resume(coroutine_t coro, void* param);
void* coroutine_yield(coroutine_t coro, void* param);
int is_coroutine_ended(coroutine_t coro);
void coroutine_destroy(coroutine_t coro);

#ifdef __cplusplus
}
#endif
