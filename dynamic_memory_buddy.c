#include <stddef.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#define N (24)
#define POOL_SIZE (1 << N)

void* calloc(size_t, size_t);
void* malloc(size_t);
void* realloc(void*, size_t);
void free(void*);

typedef struct list_t list_t;
struct list_t {
    unsigned reserved:1;
    char kval;
    list_t* succ;
    list_t* pred;
};

static list_t* freelist[N + 1] = {NULL};
static list_t* pool = NULL;

int pool_init()
{
    pool = sbrk(POOL_SIZE);
    ////printf("POOL\t%p\n", pool);
    if (pool == -1)
	return -1;

    pool->reserved = 0;
    pool->kval = N;
    pool->succ = pool->pred = pool;
    freelist[N] = pool;

    return 1;
}

void* malloc(size_t size)
{
    ////printf("MALLOC START :::::: ");
    if (size == NULL)
	return NULL;

    size += sizeof(list_t);

    if (size > POOL_SIZE)
	return NULL;

    if(!pool)
	if(pool_init() < 1)
	    return NULL;

    int kval;
    for (kval = 1; size > (1 << kval); kval++)
       ;

    int j = kval;
    list_t* block;
    while (j <= N && !(block = freelist[j]))
	j++;

    if (!block)
	return NULL;

    while (j > kval) {
	if (block->succ == block) {
	    //Pop
	    freelist[j] = NULL;
	} else {
	    //Remove from list
	    (block->pred)->succ = block->succ;
	    (block->succ)->pred = block->pred;
	    freelist[j] = block->pred;
	}
	//Init buddy
	j--;
	list_t* buddy = (char*) block + (1 << j);
	buddy->reserved = block->reserved = 0;
	buddy->kval = block->kval = j;
	buddy->succ = buddy->pred = block;
	block->succ = block->pred = buddy;

	//Insert buddy and block
	if (freelist[j]) {
	    (freelist[j]->pred)->succ = block;
	    block->pred = freelist[j]->pred;

	    buddy->succ = freelist[j];
	    freelist[j]->pred = buddy;

	    freelist[j] = block;
	} else {
	    freelist[j] = block;
	}
	//Block == freelist[j] for next iteration
    }

//Block is now freelist[kval] => pop it and use it
   if (block->succ == block) {
	//Pop
	freelist[j] = NULL;
    } else {
	//Remove from list
	(block->pred)->succ = block->succ;
	(block->succ)->pred = block->pred;
	freelist[j] = block->pred;
    }

    block->reserved = 1;
    block->pred = block->succ = block;

    //Return pointer to memory
    //printf("MALLOC\t%p\n", block);
    return (char*)block + sizeof(list_t);
}
void* calloc(size_t nmemb, size_t size)
{
    //printf("CALLOC\n");
    void* malloced_mem;
    malloced_mem = malloc(nmemb * size);
    memset(malloced_mem, 0, nmemb * size);
    return malloced_mem;
}

void merge(list_t *block)
{
    ////printf("MERGE");
    if (block->kval < N) {
	list_t* buddy = ((char*) pool + (((char*) block - (char*) pool) ^ (1 << block->kval)));
	////printf("\t%p\n", buddy);
	if (buddy->reserved = 0 && block->kval == buddy->kval) {
	    if (buddy->succ == buddy) {
		//Pop
		freelist[buddy->kval] = NULL;
	    } else {
		(buddy->pred)->succ = buddy->succ;
		(buddy->succ)->pred = buddy->pred;
		if (freelist[buddy->kval] == buddy)
		    freelist[buddy->kval] = buddy->pred;
	    }
	    //Which blocks' address is first?
	    if (block > buddy) {
		block = buddy;
		block->pred = block->succ = block;
	    }
	    block->kval++;
	    //Call recursively
	    merge(block);
	}
    } else {
	
    }
}

void free(void* ptr)
{
    if (ptr == NULL)
	return;

    list_t* block = (char*) ptr - sizeof(list_t);
    //printf("FREE\t%p\n", block);

    //Merge blocks
    merge(block);
    ////printf("MERGE DONE!\n");
    block->reserved = 0;
    if (freelist[block->kval]) {
	(freelist[block->kval]->pred)->succ = block;
	block->pred = freelist[block->kval]->pred;

	block->succ = freelist[block->kval];
	freelist[block->kval]->pred = block;

	freelist[block->kval] = block;
    } else {
	freelist[block->kval] = block;
    }
    ////printf("IN LIST\n");

}

void* realloc(void* ptr, size_t size)
{
    //printf("REALLOC\n");
    if (ptr == NULL) {
	return malloc(size);
    } else if (size == 0) {
	free(ptr);
	return NULL;
    }

    list_t* block = (char*) ptr - sizeof(list_t);

    size += sizeof(list_t);

    int kval;
    for (kval = 1; size > (1 << kval); kval++)
	;

    if (kval > block->kval) {
	//Inc mem
	void* mem = malloc(size);
	memmove(mem, ptr, (1 << block->kval) - sizeof(list_t));
	free(ptr);
	return mem;
    } else if (kval < block-kval) {
	void* mem = malloc(size);
	list_t* new_block = (char*) mem - sizeof(list_t);
	memmove(mem, ptr, size - sizeof(list_t));
	free(ptr);
	return mem;
    } else {
	return ptr;
    }
}
