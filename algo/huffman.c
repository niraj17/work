#include<stdio.h>
#include<stdlib.h>


#define PARENT(i) (i/2)
#define LEFT(i) (i*2)
#define RIGHT(i) (i*2+1)

#define MAX   20

typedef struct _heap {
	int *heap;
	int end; 
	int (*func) (void *, void *);
} heap;


int compare_int(void *d1, void *d2)
{
	int dd1 = *(int *) d1;
	int dd2 = *(int *) d2;
	return (dd1 < dd2);
}

int heap_init(heap *h, int size)
{
	h->heap = malloc(sizeof(int) * size);
	h->end = 1;
	return 0;
}
/* insert into heap */
int insert(heap *h, int new) {
	int tmp = 0, pos = 0, parent=0;
	h->heap[h->end] = new;
	pos = h->end;
	while (1) {
		parent = PARENT(pos);
		if (parent <= 0) break;
		if (h->heap[pos] < h->heap[parent]) {
			tmp = h->heap[pos];
			h->heap[pos] = h->heap[parent];
			h->heap[parent] = tmp;
			pos = parent;
		} else {
			break;
		}
	}
	h->end++;
}
int extract_min(heap *h) {
	int min = h->heap[1];
	int p, tmp, left, right, ex;

	printf("h->end == %d\n", h->end);
	h->end--; /* end usually is one beyond the last element*/
	if (h->end == 1) return h->heap[h->end];

	h->heap[1]= h->heap[h->end];
	//h->end--;

	/* set parent */
	p = 1;
	while(1) {
		left = LEFT(p);
		right = RIGHT(p);

		if (left > h->end ) break;

		if (h->heap[left] >= h->heap[p]) {
			if (right <= h->end && h->heap[right] >= h->heap[p])
				break;
		}
		if (right <= h->end ) {
			if ( h->heap[left] < h->heap[right]) { 
				ex = left;
			} else {
				ex = right;
			}
		} else {
			if (h->heap[left] < h->heap[p]) {
				ex = left;
			} else {
				break;
			}
		}
		tmp = h->heap[p];
		h->heap[p] = h->heap[ex];
		h->heap[ex]= tmp;
		p = ex;
	}
	return min;
}
int heap_test()
{
	heap h;
	int i = 0, size = 20;

	heap_init(&h, size);

	insert(&h, 9);
	insert(&h, 7);
	insert(&h, 2);
	insert(&h, 3);
	insert(&h, 8);
	insert(&h, 10);

	for (i=1; i < size; i++) {
		printf("%d ", h.heap[i]);
	}
	printf("\n");
	while((i = extract_min(&h))) {
		printf("%d ", i);
	}
}
int update_freq(char *text, int *freq)
{
	while(*text) {
		freq[*text] += 1;
		text++;
	}
}
int main(int argc, char *argv[])
{
	char *text = "This is a sample text for compression using hoffman algorithm";
	//trie t;
	int i, freq[256];
	heap h;
	heap_test();

	// init frequency
	for(i=0; i<256;i++) freq[i] = 0;
		
	heap_init(&h, 256);
	update_freq(text, freq);

	for(i='A';i<='z';i++) {
		printf("%c=%d \t", i, *(freq+i));
		insert(&h, *(freq+i));
	}
}
