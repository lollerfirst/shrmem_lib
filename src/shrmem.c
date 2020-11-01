#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/stat.h> 
#include <sys/shm.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <memory.h>
#include <stdio.h>
#include "semaphore.h"

#define MAX_SHRMEM 32
#define glibc_likely(x)       __builtin_expect((x),1)
#define glibc_unlikely(x)     __builtin_expect((x),0)

typedef struct __shrmem_block{
	char* mem_ptr;
	int32_t block_id;
	size_t size;
}Block;

__mutex_semaphore shrmem_sem;
int32_t shrmem_vector_index;
Block shrmem_vector[MAX_SHRMEM];
bool shrmem_init;

/**   ***   **/
size_t __PAGE_SIZE;
const char shrmem_reference[] = ".tmp_ref";


void init_shrmem(){
	if(glibc_likely(!shrmem_init)){
		int32_t shrmem_reference_fd = open(shrmem_reference, O_CREAT | O_RDONLY, 0440);
		close(shrmem_reference_fd);
		__PAGE_SIZE = (int) sysconf(_SC_PAGESIZE);
		shrmem_vector_index = 0;
		memset(shrmem_vector, 0x0, sizeof(shrmem_vector));
		mutex_init(&shrmem_sem);
		shrmem_init = true;
	}
}

void* get_shrmem(const int unique_id, const size_t size){
	if(__glibc_likely(shrmem_vector_index < MAX_SHRMEM && shrmem_init)){
		key_t key = ftok(shrmem_reference, unique_id);
		size_t actual_size = (size / __PAGE_SIZE) + __PAGE_SIZE;
		int32_t block_id = shmget(key, actual_size, IPC_CREAT | 0660);
		char* ptr = (char*) shmat(block_id, NULL, SHM_RND);

		if(__glibc_unlikely(key == -1 || block_id == -1 || ptr == (char*)0xffffffffffffffff))
			return (void*)0xffffffffffffffff;

		mutex_wait(&shrmem_sem);
		shrmem_vector[shrmem_vector_index].block_id = block_id;
		shrmem_vector[shrmem_vector_index].mem_ptr = ptr;
		shrmem_vector[shrmem_vector_index++].size = actual_size;
		mutex_signal(&shrmem_sem);

		return (void*)ptr;

	}else return (void*)0xffffffffffffffff;
}

void detach_shrmem(const char* addr){
	int32_t i;

	mutex_wait(&shrmem_sem);
	for(i=0; shrmem_vector[i].mem_ptr != addr && i<=shrmem_vector_index; i++);
	if(glibc_unlikely(i > shrmem_vector_index)) return;

	for(; i<=shrmem_vector_index; i++)
		shrmem_vector[i] = shrmem_vector[i+1];

	--shrmem_vector_index;
	mutex_signal(&shrmem_sem);

	shmdt(addr);
	return;
}


void deinit_shrmem(){
	if(glibc_likely(shrmem_init)){
		int32_t i;

		mutex_wait(&shrmem_sem);
		for(i=0; i<=shrmem_vector_index; i++)
			shmdt((void*) shrmem_vector[i].mem_ptr);
		shrmem_init = false;
		mutex_signal(&shrmem_sem);

		remove(shrmem_reference);
	}
}