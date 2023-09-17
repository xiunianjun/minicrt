代码来自《程序员的自我修养：链接、装载与库》第13章。

一个简单的跨WIN32平台和Linux平台的CRT（*C Runtime Library*）的C/C++实现。

对C的支持：

1. 入口函数
2. 堆管理 `malloc` / `free`
3. printf、vprintf、fprintf
4. 文件操作 `fopen` / `fclose` / `fread` / `fwrite` / `fputc` / `fputs`
5. 字符串操作

除上述外，还支持了部分C++特性：

1. string 类的实现
2. stream 类的实现，包括操纵符 endl
3. 全局对象构造和析构 cout
4. new / delete

使用步骤：

1. 编译为静态库

* C
```bash
$ gcc -m32 -c -fno-builtin -nostdlib -fno-stack-protector entry.c malloc.c stdio.c string.c printf.c
$ ar -rs minicrt.a malloc.o printf.o stdio.o string.o
```

* C++
```bash
$ gcc -m32 -c -fno-builtin -nostdlib -fno-stack-protector entry.c malloc.c stdio.c string.c printf.c atexit.c
$ g++ -m32 -c -nostdinc++ -fno-rtti -fno-exceptions -fno-builtin -nostdlib -fno-stack-protector crtbegin.cpp crtend.cpp new_delete.cpp iostream.cpp ctors.cpp
$ ar -rs minicrt.a malloc.o printf.o stdio.o string.o ctors.o atexit.o iostream.o new_delete.o
```

2. 编译测试

* C
```bash
$ gcc -m32 -c -ggdb -fno-builtin -nostdlib -fno-stack-protector test.c
$ ld -m elf_i386 -static -e mini_crt_entry entry.o test.o minicrt.a -o test
$ ./test arg1 arg2 123
```

* C++
```bash
$ g++ -m32 -c -nostdinc++ -fno-rtti -fno-exceptions -fno-builtin -nostdlib -fno-stack-protector test.cpp
ld -m elf_i386 -static -e mini_crt_entry entry.o crtbegin.o test.o minicrt.a crtend.o -o test
```
