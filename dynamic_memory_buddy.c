#include <stddef.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#define N (24)
#define POOL_SIZE (1 << N)

typedef struct list_t list_t;

struct list_t {
unsigned	reserved:1;
char	kval;
list_t* succ;
list_t* pred;
};

static list_t* freelist[N + 1];
static list_t* pool = NULL;

int init_pool() 
{
	pool = sbrk(POOL_SIZE);
	if (pool == (void*) -1)
		return -1;

	pool->reserved = 0;
	pool->kval = N;
	pool->succ = pool;
	pool->pred = pool;
	freelist[N] = pool;
	return 1;
}	

void* malloc(size_t size)
{

	if (size == NULL)
		return NULL;

	size += sizeof(list_t);

	if (size >= POOL_SIZE) {
		printf("Out of memory");
		return NULL;

	}

	if (!pool)
		if(!init_pool())
			return NULL;

	int K;
	for (K = 0; size < 2 << K; K++);
	size = 1 << K;


	int J = K;
	list_t* block;
	while (J < N && !(block = freelist[J])) 
		J++;

	if (!block)
		return NULL;

	if (block->succ == block) {
		freelist[J] = NULL;
	} else {
		(block->pred)->succ = block->succ;
		(block->succ)->pred = block->pred;
		freelist[J] = block->pred;
	}

	while (J > K) {
		
		list_t* buddy = block + (1 << (J - 1));
		buddy->kval = block->kval = block->kval - 1; 
		block->succ = block->pred = buddy;
		buddy->succ = buddy->pred = block;
		freelist[--J] = block;
		
	}

	if (block->succ == block) {
		freelist[J] = NULL;
	} else {
		(block->pred)->succ = block->succ;
		(block->succ)->pred = block->pred;
		freelist[J] = block->pred;
	}

	return block + sizeof(list_t);
}
	
	
	

void* calloc(size_t nmemb, size_t size)
{
	void* alloc_mem;

	alloc_mem = malloc(nmemb * size);
	memset(alloc_mem, 0, nmemb * size);
	return alloc_mem;

}

void* realloc(void *ptr, size_t size)
{


	
}	

list_t* merge(list_t * block) {
		if (block->kval < N) {
		list_t* buddy = pool + ((block - pool) ^ (1 << block->kval));
		if (!buddy->reserved && buddy->kval == block->kval) {
			block->kval += -1;

			if (buddy->succ == buddy) {
				freelist[block->kval] = NULL;
			} else {
				(buddy->pred)->succ = buddy->succ;
				(buddy->succ)->pred = buddy->pred;
				freelist[block->kval] = buddy->pred;
			}
		}

		if (block > buddy)
			block = buddy;

		block->kval++;
		block->pred = block->succ = block;

		return merge(block);

	}
}

void free(void* ptr)
{
	if (ptr == NULL)
		return;
	
	list_t* block = (list_t*) ((char*) ptr - sizeof(list_t));
	block = merge(block);

	block->reserved = 0;
	list_t* q = freelist[block->kval];
	if (q != NULL) {
		block->pred = q->pred;
		block->succ = q;
		(q->pred)->succ = block;
		q->pred = block;
	}
	freelist[block->kval] = block;
}

