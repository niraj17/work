/*

http://www.geeksforgeeks.org/flatten-a-linked-list-with-next-and-child-pointers/

*/

#include<stdio.h>

typedef struct node_s {
	int d;
	struct node_s *next, *down;
} node_t;


node_t * flatten(node_t *node)
{
	node_t *r1, *r2, *prev;
	if (node == NULL) return node;

	printf("Got %d\n", node->d);

	r1 =  flatten(node->down);
	r2 =  flatten(node->next);

	if (r1) {
		node->next = r1;

		//add r2 to end of r1
		while (r1) {
			prev = r1;
			r1 = r1->next;
		}
		prev->next = r2;
	} else {
		node->next = r2;
	}

	return node;
}


int main()
{
	node_t n1,n2,n3,n4, n31;
	node_t *n = &n1;

	n1.d = 1;
	n1.next = &n2;
	n1.down = &n3;

	n2.d = 2;
	n2.next = &n4;
	n2.down = NULL;

	n3.d = 3;
	n3.next = &n31;
	n3.down = NULL;

	n31.d = 31;
	n31.next = n31.down = NULL;

	n4.d = 4;
	n4.next = n4.down = NULL;

	
	flatten(&n1);

	while(n) {
		printf("%d ", n->d);
		n = n->next;
	}
}
