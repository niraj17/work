
/* http://www.geeksforgeeks.org/flipkart-interview-experience-set-40-for-sde-1/  */

#include<stdio.h>
#include<stdlib.h>

#define MAX 256
int parent[MAX];
int root;

int read_next(char *p, char *c)
{
	char  parent, child;
	int r = 0;
	r = scanf("(%c %c)", p, c);
	return r;
}

int check_cycle(int p, int c)
{
	int root = 0;
	while(p){ 
		if (p == c) {
			printf("Found cycle\n");
			break;
		}
		root = p;
		p = parent[p];
	}
	return root;
}
int find_root(int p, int c)
{
	int root = 0;
	while(p){ 
		root = p;
		p = parent[p];
	}
	return root;
}
int main()
{
	char p, c;
	int i,r=0, count = 0, root_count =0;

	while(read_next(&p, &c)) {
		printf("parent = %c, child = %c\n", p, c);
		if (parent[c] != 0) {
			printf("Multiple parent\n");
		} 
		count = 0;
		root_count = 0;
		for (i=0; i < MAX; i ++) {
			if (parent[i] == 0) continue;
			if (p == parent[i]) count++;

		}
		if (count >=2) printf("More than 2 children\n");
		if (parent[c] == p)
			printf("Duplicate tuple\n");

		r = check_cycle(p, c);
		parent[c] = p;
	}
	for (i=0; i < MAX; i ++) {
		if (parent[i] == 0) continue;
		r = find_root(parent[i], i);
		if (root == 0) 
			root = r;
		else {
			if (r != root)
				printf("Multiple root\n");
		}
	}
}	
