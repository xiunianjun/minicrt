#ifndef WIN32
typedef void (*ctor_func)(void);

// 转化-1为函数指针，标识结束
ctor_func crt_end[1] __attribute__((section(".ctors"))) = {
	(ctor_func) - 1
}
