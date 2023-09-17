/*
实现了堆管理。
malloc 和 free 的简单实现。
使用了小内存管理算法，空闲链表组织。
*/
#include "minicrt.h"

typedef struct _heap_header
{
	// 规定了该块为空闲/不空闲
    enum {
        HEAP_BLOCK_FREE = 0xABABABAB, // magic number of free block
        HEAP_BLOCK_USED = 0xCDCDCDCD, // magic number of used block
    } type; // block type: FREE/USED
	
	// 该块大小
	unsigned size; // block size including header
	// 双向链表结构
	struct _heap_header* next;
	struct _heap_header* prev;
} heap_header;

#define ADDR_ADD(a,o) (((char*)(a)) + o)
#define HEADER_SIZE (sizeof(heap_header))

// 记录堆头
static heap_header* list_head = NULL;

void free(void* ptr);
void* malloc(unsigned size);
#ifndef WIN32
// Linux brk syscall
static int brk(void* end_data_segment) {
	int ret = 0;
	// brk syscall num: 45
	asm("movl	$45,	%%eax	\n\t"
		"movl	%1,		%%ebx	\n\t"
		"int	$0x80			\n\t"
		"movl	%%eax,	%0		\n\t"
		: "=r"(ret): "m"(end_data_segment)
	);
}
#endif

#ifdef WIN32
#include <Windows.h>
#endif

int mini_crt_init_heap()
{
	void* base = NULL;
	heap_header *header = NULL;
	// 32MB
	unsigned heap_size = 1024 * 1024 * 32;
#ifdef WIN32
	base = VirtualAlloc(0, heap_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (base == NULL)	return 0;
#else
	// brk系统调⽤可以设置进程的数据段边界
	// 获取堆的开始
	base = (void*)brk(0);
	// 将堆向后推32MB，并且获取堆的结束位置
	void* end = ADDR_ADD(base, heap_size);
	end = (void*)brk(end);
	if (!end)	return 0;
#endif
	// 一开始，只有一大坨堆头
	header = (heap_header *)base;

	header->size = heap_size;
	header->type = HEAP_BLOCK_FREE;
	header->next = NULL;
	header->prev = NULL;
	list_head = header;
	return 1;
}

void free(void* ptr)
{
	heap_header *header = (heap_header *)ADDR_ADD(ptr, -HEADER_SIZE);
	if (header->type != HEAP_BLOCK_USED) {
		// 重复free，不管
		return ;
	}
	
	header->type = HEAP_BLOCK_FREE;
	// 合并后方
	if (header->prev && header->prev->type == HEAP_BLOCK_FREE) {
		header->prev->next = header->next;
		if (header->next)	header->next->prev = header->prev;
		header->prev->size += header->size;
		header = header->prev;
	}

	// 合并前方
	if (header->next && header->next->type == HEAP_BLOCK_FREE) {
		header->size += header->next->size;
		header->next = header->next->next;
	}
}
void* malloc(unsigned size)
{
	heap_header *header;
	if (size == 0)	return NULL;

	for (header = list_head; header; header = header->next) {
		if (header->type != HEAP_BLOCK_FREE)	continue;
		
		// 如果该块大小位于 所需空间 ~ 所需空间 + header 之间
		// 此时是不能再切分块为 used + free 的，因为这样的话需要两个header
		// 而这空间不够存两个 header
		if (header->size > size + HEADER_SIZE 
			&& header->size <= size + HEADER_SIZE * 2) {
			header->type = HEAP_BLOCK_USED;
			return ADDR_ADD(header, HEADER_SIZE);	
		}		

		if (header->size > size + HEADER_SIZE * 2) {
			// 切割
			heap_header *next = (heap_header *)ADDR_ADD(header, size + HEADER_SIZE);
			next->prev = header;
			next->next = header->next;
			next->type = HEAP_BLOCK_FREE;
			next->size = header->size - (size + HEADER_SIZE);
			
			header->next = next;
			header->size = size + HEADER_SIZE;
			header->type = HEAP_BLOCK_USED;
			return ADDR_ADD(header, HEADER_SIZE);			
		}
	}
	return NULL;
}

