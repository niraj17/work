#include<stdio.h>


typedef struct node {
	int data;
	struct node *next;
} node_t;


int print_list(node_t *n)
{
	while(n) {
		printf(" %d ", n->data);
		n = n->next;
	}
	return 0;
}

node_t *rev_list(node_t *n)
{
	node_t *prev = NULL;
	node_t *current = n;
	node_t *next = NULL;

	while (current) {
		next = current->next;

		current->next = prev;

		prev = current;
		current = next;
	}
	
	return prev;
}
int main(int argc, char *argv[])
{
	node_t n1,n2,n3;
	node_t *n;

	n1.data = 1;
	n2.data = 2;
	n3.data = 3;

	n1.next = &n2;
	n2.next = &n3;
	n3.next = NULL;

	print_list(&n1);

	n = rev_list(&n1);
	print_list(n);
}
