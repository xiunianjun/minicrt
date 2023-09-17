#ifndef WIN32
typedef void (*ctor_func)(void);

// 转化-1为函数指针，标识结束
ctor_func ctors_begin[1] __attribute__((section(".ctors"))) = {
	(ctor_func)-1
}

void run_hooks()
{
	const ctor_func *list = ctors_begin;
	// 逐个调用ctors_begin里的东西
	while ((int)*++list != -1)	(**list)();
}
#endif
