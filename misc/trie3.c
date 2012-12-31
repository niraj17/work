/*
 * A very very simple Trie based search for matching multiple filters.
 *
 * Adding a new filter requires recompilation, see beginning of main function.
 * Add the new filter using 'add_filter' function call in main and recompile.	
 * Note : '*' is used to represent 'dunno' (don't care) bit.
 *
 * To search, give the input as command line argument.
 *   ./a.out <search_string>
 *	
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

/* BITS_PER_TREE denotes how many bits a single tree takes care of.
 * TREE_COUNT is the number of tree you will need.
 * Example-  if you need to have 64 bits filters, then you can have
 * BITS_PER_TREE as 4 and TREE_COUNT as 16.
 * You can play around with these values.
 */

#define BITS_PER_TREE 4
#define TREE_COUNT    16

struct node {
	unsigned int bits;
	struct node *n1, *n3; /*n1->'0', n3->'1'*/
};

struct node root[TREE_COUNT];
int node_count;

int add_filter_int(int id, struct node *node, char *s)
{
	struct node **n = NULL;

	if (*s == 0) {
		node->bits |= (0x1 << id);
		return 0;
	}

	if (*s == '*') {
		n = &(node->n1); 
		if (*n == NULL ) {
			*n = malloc (sizeof(struct node));
			memset(*n, 0, sizeof(struct node));
			node_count++;
		}
		add_filter_int(id, *n, s+1);

		n = &(node->n3); 
		if (*n == NULL ) {
			*n = malloc (sizeof(struct node));
			memset(*n, 0, sizeof(struct node));
			node_count++;
		}
		add_filter_int(id, *n, s+1);
		return 0;
	}
	if (*s == '0') n = &(node->n1); 
	if (*s == '1') n = &(node->n3); 

	if (*n == NULL ) {
		*n = malloc (sizeof(struct node));
		memset(*n, 0, sizeof(struct node));
		node_count++;
	}
	add_filter_int(id, *n, s+1);
	return 0;
}
	

int add_filter(int filter_id, char *s)
{
	int i = 0;
	char ss[10];

	printf("Adding filter: %d: %s\n", filter_id, s);
	--filter_id;

	ss[BITS_PER_TREE] = 0;
	for(i=0; i < TREE_COUNT; i++) {
		memcpy(ss, s+i*BITS_PER_TREE, BITS_PER_TREE);
		add_filter_int(filter_id, &root[i], ss);
	}
}

int search_filter_int(struct node *node, char *s)
{
	struct node *n = NULL;

	if(*s == 0) return node->bits;
	if (*s == '0') n = node->n1; 
	if (*s == '1') n = node->n3; 
	if (n == NULL) return 0;
	return search_filter_int(n, s+1);

}
int search_filter(char *s)
{
	unsigned int result = 0 , ret = 0, i = 0;
	char ss[10];

	ss[BITS_PER_TREE] = 0;

        // set all bits of result
        result = ~result; 

	for(i=0; i < TREE_COUNT; i++) {
		memcpy(ss, s+i*BITS_PER_TREE, BITS_PER_TREE);
		ret = search_filter_int(&root[i], ss);
		//printf("result = %d\n", ret);
		if (ret == 0) return ret;
		result &= ret;
	}
	return result;
}

int main(int argc, char *argv[])
{
	int ret = 0, i = 0;

	if(argc < 2) {
		printf("Usage: ./a.out <search_string>\n\n");
		exit(1);
	}

	/*             <----------- 64 bits ------------------------------------------> */
	//             1234567812345678123456781234567812345678123456781234567812345678
	add_filter(1, "0111111101010101111111111111111101111111010101011111111111111111");
	add_filter(2, "0111111111111111111111111111111111111111111111111111111111111111");
	add_filter(7, "**************************************************************1*");
	add_filter(8, "****************************************************************");

	printf("Node count = %d\n", node_count);
	printf("---------------\n");
	printf("Searching for input %s\n", argv[1]);
	ret = search_filter(argv[1]);
	printf("Matched filters : ");
	for (i=0; i < 32; i++) {
		if(ret & (0x1 << i)) printf("%d ", i+1);
	}
	printf("\n");
}
