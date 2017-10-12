#include <stdio.h>
#include <stdlib.h>
#include "asm-impl/coroutine.h"

void* entry_point(coroutine_t parent, void* value)
{
    puts("coroutine says 1");
    value = coroutine_yield(parent, value);
    puts("coroutine says 2");
    value = coroutine_yield(parent, value);
    puts("coroutine says 3");
    value = coroutine_yield(parent, value);
    puts("coroutine says 4");
    return value;
}

int main()
{
    // Small text
    coroutine_t coro = coroutine_new(entry_point, 4096);
    
    int v = 0;
    void* value = &v;
    puts("main says 1");
    value = coroutine_resume(coro, value);
    puts("main says 2");
    value = coroutine_resume(coro, value);
    puts("main says 3");
    value = coroutine_resume(coro, value);
    puts("main says 4");
    value = coroutine_resume(coro, value);
    
    coroutine_destroy(coro);
    return 0;
}
