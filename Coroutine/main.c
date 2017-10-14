#include <stdio.h>
#include <stdlib.h>
#include "asm-impl/coroutine.h"

const void* entry_point(coroutine_t parent, const void* value)
{
	int n = 0;
	printf("coroutine receives %s from main (n = %d)\n", value, n);
	n++;
	value = coroutine_yield(parent, "cr1");   
	printf("coroutine receives %s from main (n = %d)\n", value, n);
	n++;
	value = coroutine_yield(parent, "cr2");
	printf("coroutine receives %s from main (n = %d)\n", value, n);
	n++;
	value = coroutine_yield(parent, "cr3");
	printf("coroutine receives %s from main (n = %d)\n", value, n);
	return "cr4";
}

int main()
{
	// Small text
	coroutine_t coro = coroutine_new(entry_point, 4096);
    
	const void* value;
	int m = 7;
	value = coroutine_resume(coro, "mn1");
	printf("main receives %s from coroutine (m = %d)\n", value, m);
	m--;
	value = coroutine_resume(coro, "mn2");
	printf("main receives %s from coroutine (m = %d)\n", value, m);
	m--;
	value = coroutine_resume(coro, "mn3");
	printf("main receives %s from coroutine (m = %d)\n", value, m);
	m--;
	value = coroutine_resume(coro, "mn4");
	printf("main receives %s from coroutine (m = %d)\n", value, m);
    
	coroutine_destroy(coro);
	return 0;
}
