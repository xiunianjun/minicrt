代码来自《程序员的自我修养：链接、装载与库》第13章。
一个简单的跨WIN32平台和Linux平台的CRT（*C Runtime Library*）的C/C++实现。

使用步骤：
1. 编译为静态库
* Linux
```bash
$ gcc -m32 -c -fno-builtin -nostdlib -fno-stack-protector entry.c malloc.c stdio.c string.c printf.c
$ ar -rs minicrt.a malloc.o printf.o stdio.o string.o
```
* Windows
```
>cl /c /DWIN32 /GS- entry.c malloc.c printf.c stdio.c string.c
>lib entry.obj malloc.obj printf.obj stdio.obj string.obj /OUT:minicrt.lib
```
2. 编译测试
```bash
$ gcc -m32 -c -ggdb -fno-builtin -nostdlib -fno-stack-protector test.c
$ ld -m elf_i386 -static -e mini_crt_entry entry.o test.o minicrt.a -o test
$ ./test arg1 arg2 123
```
