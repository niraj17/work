/*
 http://www.geeksforgeeks.org/minimum-number-of-jumps-to-reach-end-of-a-given-array/
*/
#include<stdio.h>
#include<limits.h>


int min(int x, int y) { return (x < y)? x: y; }


int min_jumps(int arr[], int size)
{
	int jumps[size];

	int i, j;

	jumps[0] = 0;

	for(i=1; i<size; i++) {
		jumps[i] = INT_MAX;

		for(j = 0; j < i; j++) {
			if (i <= j+arr[j] && jumps[j] != INT_MAX ) {
				jumps[i] = min(jumps[i], jumps[j] + 1);
				break;
			}
		}
	}
	return jumps[size-1];

}

int main()
{
    int arr[] = {1, 3, 6, 1, 0, 9};
    int size = sizeof(arr)/sizeof(int);
    printf("Minimum number of jumps to reach end is %d \n", min_jumps(arr,size));
    return 0;
}
