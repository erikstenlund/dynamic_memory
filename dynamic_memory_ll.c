#include <stddef.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>


typedef struct list_t list_t;

void free(void*);

struct list_t {
	size_t size;
	list_t* next;
	char data[];
};

static list_t avail = { .next = NULL };

void* malloc(size_t size)
{

	if (size == NULL)
		return NULL;

	list_t *p, *q;

	p = &avail;
	q = p->next;
	while (q != NULL && q->size <= size) {
		p = q;
		q = q->next;
	}
	void* new_mem;
	if (q == NULL) {
		//No sufficiently large block found, ask kernel for more
		new_mem = sbrk(size + sizeof(list_t));
		if (new_mem == (void*) -1)
			return NULL;




		q = (list_t*) new_mem;
		//p->next = q;
		q->next = NULL;
		q->size = size;
	} else {
		p->next = q->next;
		q->next = NULL;

	}

	return q->data;
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
		free(ptr); //VÄL? 	
		return mem;
		
	}


	
}	


void free(void* ptr)
{
	if (ptr == NULL)
		return;
	
	list_t* r = (void*)ptr - sizeof(list_t);

	list_t *p, *q;

	p = &avail;
	q = p->next;

	while (q != NULL) {
		p = q;
		q = q->next;
	}

	p->next = r;
	r->next = NULL; 
}



