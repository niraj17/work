
/*
 http://www.geeksforgeeks.org/topological-sorting/
*/
#include<stdio.h>
#include<stdlib.h>

#define MAX 1024

typedef struct adj_s {
	int dest;
	struct adj_s *next;
} adj_t;

typedef struct graph_s {
	int v;
	struct adj_s **nodes; /* array of list*/
} graph_t;


typedef struct stack_s {
	int d[MAX];
	int top;
} stack_t;

typedef struct queue_s {
	int d[MAX];
	int front, rear;
} queue_t;

graph_t * create_graph(int v) {
	int i = 0;

	graph_t *g = malloc(sizeof(graph_t));

	g->v = v;
	g->nodes = malloc(v * sizeof(adj_t *));

	for(i=0; i < v; i++)
		g->nodes[i] = NULL;
	
	return g;
}

int add_directed_edge(graph_t *g, int src, int dest) {
	adj_t *n = malloc(sizeof(adj_t));	

	n->dest = dest;
	n->next = g->nodes[src];

	/* add to the front of the list*/
	g->nodes[src] = n;

	return 0;
}

int print_graph(graph_t *g)
{
	int i = 0;
	adj_t *n = NULL;
	for(i=0; i < g->v; i++)
	{
		printf("%d : ", i);
		n = g->nodes[i];
		while(n) {
			printf("%d ", n->dest);
			n = n->next;
		}
		printf("\n");
	}
}


topo_sort_helper(graph_t *g, int n, int visited[], stack_t *st) 
{
	adj_t *a = g->nodes[n];
	
	visited[n] = 1;

	while(a) {
		if (visited[a->dest] == 0) {
			topo_sort_helper(g, a->dest, visited, st);
		}
		a = a->next;
	}

	/* push to stack */
	st->top++;
	st->d[st->top] = n;

}
topo_sort(graph_t *g) 
{
	int i, visited[MAX] = {0};
	stack_t st;
	st.top = 0;
	
	for(i=0; i < g->v; i++)
	{
		if (visited[i] == 0) {
			topo_sort_helper(g, i, visited, &st);
		}
	}

	/* print the stack */
	printf("Topological sort: ");
	for(i = st.top; i > 0; i--)
	{
		printf("%d ", st.d[i]);
	}
	printf("\n");
}

int graph_bfs(graph_t *g, int v)
{
	queue_t q;
	visited[g->v] = {0};

	q.front = q.rear = 0;

	// add to the queue
	q.d[q.rear] = v;
	q.rear++;

	visited[v] = 1;

	while(q.rear != q.front) {
		//remove from queue
		vv = q.d[q.front];
		q.front++;
		printf("%d ", vv);


		//Add all nodes adjecent to vv to the queue if not visited
		

	}

}

int main()
{
        int v = 6;
        graph_t *g = create_graph(v);

        add_directed_edge(g, 5 , 2);
        add_directed_edge(g, 5 , 0);
        add_directed_edge(g, 4 , 0);
        add_directed_edge(g, 4 , 1);
        add_directed_edge(g, 2 , 3);
        add_directed_edge(g, 3 , 1);

	print_graph(g);

	topo_sort(g);
}
