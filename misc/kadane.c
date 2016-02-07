/*
	http://www.geeksforgeeks.org/largest-sum-contiguous-subarray/
*/

#include<stdio.h>

#define max(x,y) (x>y?x:y)

int find_max_sum(int arr[], int n)
{
	int i = 0;
	int max_ending_here=0, max_so_far = 0;

	for(i=0;i < n; i++)
	{
		max_ending_here = max(0, max_ending_here+arr[i]);
		max_so_far      = max(max_so_far, max_ending_here);
	}
	return max_so_far;
}


int main()
{
	int a[] = {-2, -3, 4, -1, -2, 1, 5, -3};
	int r = find_max_sum(a, sizeof(a)/sizeof(a[0]));

	printf("Ans is %d\n", r);
}
