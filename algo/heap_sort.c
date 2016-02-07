#include<stdio.h>


#define PARENT(i) (i/2)
#define LEFT(i) (i*2)
#define RIGHT(i) (i*2+1)

#define MAX   20
int heap[MAX];
int end = 1;

/* insert into heap */
int insert(int new) {
	int tmp = 0, pos = 0, parent=0;
	heap[end] = new;
	pos = end;
	while (1) {
		parent = PARENT(pos);
		if (parent <= 0) break;
		if (heap[pos] < heap[parent]) {
			tmp = heap[pos];
			heap[pos] = heap[parent];
			heap[parent] = tmp;
			pos = parent;
		} else {
			break;
		}
	}
	end++;
}
int extract_min(void) {
	int min = heap[1];
	int p, tmp, left, right, ex;


	/* no more elements */
	if (end == 1) return 0;

	end--; /* end is one beyond the last element*/
	if (end == 1) return heap[end];

	heap[1]= heap[end];
	//end--;

	/* set parent */
	p = 1;
	while(1) {
		left = LEFT(p);
		right = RIGHT(p);

		if (left > end ) break;

		if (heap[left] >= heap[p]) {
			if (right <= end && heap[right] >= heap[p])
				break;
		}
		if (right <= end ) {
			if ( heap[left] < heap[right]) { 
				ex = left;
			} else {
				ex = right;
			}
		} else {
			if (heap[left] < heap[p]) {
				ex = left;
			} else {
				break;
			}
		}
		tmp = heap[p];
		heap[p] = heap[ex];
		heap[ex]= tmp;
		p = ex;
	}
	return min;
}
int main(int argc, char *argv[])
{
	int i = 0;

	insert(10);
	insert(9);
	insert(7);
	insert(2);
	insert(3);
	insert(8);
	insert(4);

	for (i=1; i < end; i++) {
		printf("%d ", heap[i]);
	}
	printf("\n");
	while((i = extract_min())) {
		printf("%d ", i);
	}
}
