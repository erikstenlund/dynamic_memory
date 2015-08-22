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

void init_pool() 
{
	pool = sbrk(POOL_SIZE);
	if (pool == (void*) -1)
		return;

	pool = {{.reserved = 0, .kval = N, .succ = pool, .pred = pool}};
	freelist[N] = pool;
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
		freelist[J] = NULL
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
		freelist[J] = NULL
	} else {
		(block->pred)->succ = block->succ;
		(block->succ)->pred = block->pred;
		freelist[J] = block->pred;
	}

	return p + sizeof(list_t);

	
	
	

void* calloc(size_t nmemb, size_t size)
{
	void* alloc_mem;

	alloc_mem = malloc(nmemb * size);
	memset(alloc_mem, 0, nmemb * size);
	return alloc_mem;

}

void* realloc(void *ptr, size_t size)
{
	if (ptr == NULL) {
		return malloc(size);

	} else if (size == 0) {
		
		free(ptr);
		return NULL;
	}

	list_t* r = ptr - sizeof(list_t);

	if (r->size >= size) { //reduce size of memory
		if (r->size - size <= 3 * sizeof(list_t)) {
			//Its not worth it
			return ptr;
		} else {
			void* mem = malloc(size);
			memcpy(mem, r->data, size);
			free(ptr);
			return mem;
		}

	} else {	//increase size of memory
		void* mem = malloc(size);
		memcpy(mem, r->data, r->size);
		
		return mem;
		
	}


	
}	


void free(void* ptr)
{
	if (ptr == NULL)
		return;
	
	list_t* block = (list_t*) ((char*) ptr - sizeof(list_t));
	block = merge(block);

	block->reserved = 0;
	list_t* q = freelist[i];
	if (q != NULL) {
		block->pred = q->pred;
		block->succ = q;
		(q->pred)->succ = block;
		q->pred = block;
	}
	freelist[block] = block;
}

list_t *merge(list_t *list_ptr) {
		if (block->kval < N) {
		list_t* buddy = pool + ((ptr - pool) ^ (1 << block->kval));
		if (!buddy->reserved && buddy->kval == block->kval) {
			block->kval += -1;

			if (buddy->succ == buddy) {
				freelist[J] = NULL
			} else {
				(buddy->pred)->succ = buddy->succ;
				(buddy->succ)->pred = buddy->pred;
				freelist[J] = buddy->pred;
			}
		}

		if (block > buddy)
			block = buddy;

		block->kval++;
		block->pred = block->succ = block;

		return merge(block);

	}
}