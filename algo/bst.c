/*
 * Verify that given tree is BST
 */

#include<stdio.h>

#define MAX  9999
#define MIN  0


typedef struct node_s {
	struct node_s* left, *right;
	int d;
} node_t;

int verify_bst(node_t *n, int min, int max)
{
	int r = 0, l = 0;
	if (n == NULL) return 0;

	if (n->d < min || n->d > max) return 1;

	l = verify_bst(n->left, min, n->d);
	r = verify_bst(n->right, n->d, max);

	if (r != 0 || l != 0) return 1;
	return 0;
}

node_t * find_successor(node_t *root, node_t *node)
{
	node_t *t = root, *succ;
	if (node->right) {
		return find_minimum(node->right);
	} 
	while (t) {
		if (t->d > node->d) {
			succ = t;
			t = t->left;
		} else if (t->d < node->d) {
			t = t->right;
		}
		else {
			break;
		}
	}
	return succ;

}

int main() {
	int result  = 0;

	node_t n1, n2, n3, n4;

	n1.d = 20;
	n1.left = &n2;
	n1.right = &n3;

	n2.d = 5;
	n2.left = NULL;
	n2.right = &n4;


	n3.d = 25;
	n3.left = NULL;
	n3.right = NULL;

	n4.d = 9;
	n4.left = NULL;
	n4.right = NULL;

	result = verify_bst(&n1,  MIN, MAX);
	printf("result is %d\n", result);
}
