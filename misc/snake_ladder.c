/*
http://www.geeksforgeeks.org/snake-ladder-problem-2/
*/

#include<stdio.h>

int find_min_steps(int board[], int n)
{
	int i, visited[n];
	int queue[100], start= 0, end = 0;
	int dist[100] = {0};

	int node = 0, nn = 0;
	int count = 0;

	for(i=0; i < n; i++)
		visited[i] = 0;

	// add the first node to queue
	visited[node]=1;
	queue[end] = node;
	end++;

	while (end != start) {
		
		// remove node from queue
	        node = queue[start];
		start++;

		// check if this is the final node
		if (node == n-1) break;

		for(i=node+1; i<=node+6; i++) {

			if (visited[i] == 1) continue;
			if (i >= 30) continue;
			
			if (board[i] == -1) {
				nn = i;
			} else {
				nn = board[i];
			}

			// add nn to queue
			visited[nn] = 1;
			dist[end] = dist[node] + 1;
			queue[end] = nn;
			end++;
		}
	}

	return dist[n-1];
}



int main()
{
	int i, b[30] = {-1};

	// 
	for(i = 0; i < 30; i++)
		b[i] = -1;

	// Ladders
	b[2] = 21;
	b[4] = 7;
	b[10] = 25;
	b[19] = 28;

	// Snakes
	b[26] = 0;
	b[20] = 8;
	b[16] = 3;
	b[18] = 6;

	printf("Min steps = %d\n", find_min_steps(b, 30));

}
