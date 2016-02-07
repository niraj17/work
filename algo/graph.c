#include<stdio.h>
#include<stdlib.h>

#define MAX 100

typedef struct stack_s {
	void *p[MAX];
	int top;
} stack_t;
	

typedef struct adj_node_s {
	int dest;
	struct adj_node_s *next;
} adj_node_t;

typedef struct graph_s {
	int v;
	adj_node_t **arr; /* array of list */
} graph_t;


graph_t *create_graph(int v)
{
	graph_t *g;
	int i = 0;

	g = malloc(sizeof (graph_t));
	g->v = v;

	g->arr = malloc(v * sizeof (adj_node_t *));

	for(i=0; i < v; i++)
		g->arr[i] = NULL;

	return g;
}

int add_edge(graph_t *g, int src, int dest)
{
	adj_node_t *tmp = NULL, *prev = NULL;
	adj_node_t *l = malloc (sizeof(adj_node_t));
	l->next = NULL;
	l->dest = dest;

	/* Add to the front of the list*/
	tmp = g->arr[src];
	g->arr[src] = l;
	l->next = tmp;

	/* for undirected graph - Add another one from dest to src */
	l = malloc (sizeof(adj_node_t));
	l->next = NULL;
	l->dest = src;

	tmp = g->arr[dest];
	g->arr[dest] = l;
	l->next = tmp;
}

int print_graph(graph_t *g)
{
	int i;
	adj_node_t *n;

	for(i=0; i < g->v; i++) {
		printf("\nAdj list of vertex %d : ", i);
		n = g->arr[i];
		while(n) {
			printf(" %d ", n->dest);
			n = n->next;
		}
	}
	printf("\n");
}
int topo_helper(adj_node_t *n, stack_t *st)
{


}
int topological_sort(graph_t *g)
{
	for(


}


int main(int argc, char *argv[])
{
	int v = 5;
	graph_t *g = create_graph(v);

	add_edge(g, 0 , 1);
	add_edge(g, 0 , 4);
	add_edge(g, 1 , 2);
	add_edge(g, 1 , 3);
	add_edge(g, 1 , 4);
	add_edge(g, 2 , 3);
	add_edge(g, 3 , 4);

	print_graph(g);
}
