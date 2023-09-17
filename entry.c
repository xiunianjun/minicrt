#include "minicrt.h"
#ifdef WIN32
#include <Windows.h>
#endif

extern int main(int argc, char* argv[]);
void exit(int);
static void crt_fatal_error(const char* msg)
{
    // printf("fatal error: %s", msg);
    exit(1);
}

void mini_crt_entry(void)
{
    int ret;

    // 构造main的参数
#ifdef WIN32

    int flag = 0;
    int argc = 0;
    char* argv[16]; // 最多16个参数
    char* cl = GetCommandLineA();

    // 解析命令⾏
    argv[0] = cl;
    argc++;
    while(*cl) {
        if(*cl == '\"')
        if(flag == 0) flag = 1;
        else flag = 0;
        else if(*cl == ' ' && flag == 0) {
            if(*(cl+1)) {
                argv[argc] = cl + 1;
                argc++;
            }
            *cl = '\0';
        }
        cl++;
    }

#else

    int argc;
    char** argv;
    char* ebp_reg = 0;
    // ebp_reg = %ebp
    asm("movl %%ebp,%0 \n":"=r"(ebp_reg));
    argc = *(int*)(ebp_reg + 4);
    argv = (char**)(ebp_reg + 8);

#endif

    // 构造堆
    if (!mini_crt_init_heap())
        crt_fatal_error("heap initialize failed");
    // 初始化io
    if (!mini_crt_init_io())
    	crt_fatal_error("IO initialize failed");

	// 构造所有全局对象
	do_global_ctors();

    ret = main(argc,argv);
    exit(ret);
}


void exit(int exitCode)
{
	// 执行atexit，完成所有finit钩子
	mini_crt_call_exit_routine();
#ifdef WIN32
    ExitProcess(exitCode);
#else
    // 调用exit系统调用
    asm( "movl %0,%%ebx \n\t"
    "movl $1,%%eax \n\t"
    "int $0x80 \n\t" 
    "hlt \n\t"::"m"(exitCode));
#endif
}
