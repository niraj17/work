#include<stdio.h>

#define MAX 20

typedef struct node_s
{	
	int data;
	struct node_s *left, *right;
} node_t;


node_t *queue[MAX];

int level_print(node_t *n)
{
	node_t *tmp = n;
	int i = 0, e = 0;
	
	do {
		if (tmp != NULL) {
			printf("%d ", tmp->data);
			if (tmp->left) {
				queue[e] = tmp->left;
				e++;
			}
			if (tmp->right) {
				queue[e] = tmp->right;
				e++;
			}
		}	

		tmp = queue[i];
		i++;
	} while(tmp);
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

	level_print(&n1);
}
