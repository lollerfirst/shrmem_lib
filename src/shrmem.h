#pragma once
#include "semaphore.h"
#include <stdbool.h>

#define MAX_SHRMEM 32


typedef struct __shrmem_block{
	char* mem_ptr;
	int32_t block_id;
	size_t size;
}Block;

extern __mutex_semaphore shrmem_sem;
extern int32_t shrmem_vector_index;
extern Block shrmem_vector[MAX_SHRMEM];
extern bool shrmem_init;

/**   ***   **/
extern size_t __PAGE_SIZE;
extern const char shrmem_reference[];

extern void init_shrmem();
extern void* get_shrmem(const int, const size_t);
extern void detach_shrmem(const char*);
extern void deinit_shrmem();