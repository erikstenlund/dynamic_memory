#include <stddef.h>
#include <unistd.h>
#include <string.h>



struct list_t {
	size_t size;
	list_t* next;
	char data[];
};

typedef struct list_t list_t;

static list_t avail = { .next = NULL };

void* malloc(size_t size)
{
	if (size == NULL)
		return NULL;

	list_t *p, *q;

	p = &avail;
	q = p->next;

	while (q != NULL || q->size <= size) {
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
		q->next = NULL;
		q->size = size;
	} else {
		//q is big enough, use it
		p->next = q->next;


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

void free(void* ptr)
{
	if (ptr == NULL)
		return;
	
	list_t* r = ptr - sizeof(list_t);
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



