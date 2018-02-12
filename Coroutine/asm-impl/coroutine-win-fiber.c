#include "coroutine.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdlib.h>

typedef struct
{
	coroutine_entry_point entry_point;
	LPVOID fiber;
	const void* param_exchanged;
} fiber_data;

inline static const void* _cryield(coroutine_t coro, const void* param)
{
	fiber_data* data = (fiber_data*)coro;
	data->param_exchanged = param;
	LPVOID fiber = data->fiber;
	data->fiber = GetCurrentFiber();
	SwitchToFiber(fiber);
	return data->param_exchanged;
}

static VOID CALLBACK fiber_entry_point(PVOID param)
{
	fiber_data* data = (fiber_data*)param;
	const void* last = data->entry_point(data, data->param_exchanged);
	data->entry_point = 0;
	_cryield(data, last);
}

coroutine_t coroutine_new(coroutine_entry_point point, size_t stack_size)
{
	if (!IsThreadAFiber())
		if (!ConvertThreadToFiberEx(NULL, FIBER_FLAG_FLOAT_SWITCH))
			return NULL;

	fiber_data* data = malloc(sizeof(fiber_data));
	if (!data) return 0;
	data->entry_point = point;

	LPVOID fiber = CreateFiberEx(stack_size, stack_size, FIBER_FLAG_FLOAT_SWITCH, fiber_entry_point, data);
	if (!fiber)
	{
		free(data);
		return NULL;
	}

	data->fiber = fiber;

	return data;
}

const void* coroutine_resume(coroutine_t coro, const void* param)
{
	return coroutine_yield(coro, param);
}

const void* coroutine_yield(coroutine_t coro, const void* param)
{
	if (!coro) return NULL;
	fiber_data* data = (fiber_data*)coro;
	if (!data->entry_point) return NULL;

	return _cryield(coro, param);
}

int is_coroutine_ended(coroutine_t coro)
{
	if (!coro) return TRUE;
	return !((fiber_data*)coro)->entry_point;
}

void coroutine_destroy(coroutine_t coro)
{
	if (!coro) return;
	if (!is_coroutine_ended(coro)) return;

	fiber_data* data = (fiber_data*)coro;
	DeleteFiber(data->fiber);
	free(data);
}