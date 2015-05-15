#include <stddef.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

typedef struct list_t list_t;

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
	printf("Realloc\n");
	if (ptr == NULL) {

		return malloc(size);
	} else if (size == 0) {
		
		free(ptr);
		return;
	}

	list_t* r = ptr - sizeof(list_t);

	if (r->size >= size) {
		if (r->size - size >= 3 * sizeof(list_t)) {
			//Its not worth it
			return r;
		} else {
			printf("Realloc2\n");
			list_t *p, *q;
			p = &avail;
			q = p->next;
			while (q != NULL) {
				p = q;
				q = q->next;
			}

			q = (list_t*) r + sizeof(list_t) + size;
			//p->next = q;
			q->next = NULL;
			q->size = r-size - size;
			r->size = size;

			return r;
		}

	} else {
					printf("Realloc3\n");

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
			q->next = NULL;
			q->size = size;
		} else {
			p->next = q->next;
			q->next = NULL;

		}
printf("Realloc4\n");
		memcpy(r, q + sizeof(list_t), r->size);
		free(r);
		printf("Realloc5\n");
		return q;
		
	}


	
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



