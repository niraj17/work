/*
	http://www.geeksforgeeks.org/dynamic-programming-set-6-min-cost-path/
*/
#include<stdio.h>

int min_cost(int arr[][], int r, int c)
{
	int cost[5][5];
	int i, j ;

	for(i=1; i<r;i++) {
		for(j=1; j < c; j++) {
			cost[i][j] = min(cost[i][j-1] + arr[i][j],
					cost[i-1][j] + arr[i][j],
					cost[i-1][j-1] + arr[i][j])
		}
	}

}

int main()
{
	int arr[5][5] = {};
	int c = 0;

	c = min_cost(arr, 5, 5);
}
