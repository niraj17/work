#include<stdio.h>

#define MAX 20

typedef struct node_s
{	
	int data;
	struct node_s *left, *right;
} node_t;


int fix_left(node_t *root)
{
	static node_t *prev = NULL;
	if (root == NULL) return;

	fix_left(root->left);
	root->left = prev;
	prev = root;
	
	fix_left(root->right);
}

node_t *fix_right(node_t *root)
{
	node_t *n = root;
	node_t *prev = NULL;

	while(n && n->right)
		n = n->right;

	while(n && n->left) {
		prev = n->left;
		prev->right = n;
		n = prev;
	}

	return n;
}

node_t * tree_to_dll(node_t *root)
{
	fix_left(root);

	return fix_right(root);
}

int print_dll(node_t *n)
{
	while(n) {
		printf(" %d", n->data);
		n = n->right;
	}
}

int main(int argc, char *argv[])
{
	node_t n1,n2,n3,n4,n5;

	n1.data = 1;
	n1.left = &n2;
	n1.right = &n3;

	n2.data = 2;
	n2.left = &n4;
	n2.right = NULL;

	n3.data = 3;
	n3.left = NULL;
	n3.right = &n5;

	n4.data = 4;
	n4.left = NULL;
	n4.right = NULL;

	n5.data = 5;
	n5.left = NULL;
	n5.right = NULL;

	print_dll(tree_to_dll(&n1));
}
