typedef void (*init_func)(void);
#ifdef WIN32
#pragma section(".CRT$XCA", long, read)
#pragma section(".CRT$XCZ", long, read)

_declspec(allocate(".CRT$XCA")) init_func
ctors_begin[] = { 0 };
_declspec(allocate(".CRT$XCZ")) init_func
ctors_begin[] = { 0 };

extern "C" void do_global_ctors()
{
	init_func *p = ctors_begin;
	while (p < ctors_end)
	{
		if (*p !=0 )	(**p)();
		++p;
	}
}
#else

void run_hooks();
extern "C" void do_global_ctors()
{
	run_hooks();
}
#endif
