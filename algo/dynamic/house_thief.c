/*
http://www.geeksforgeeks.org/flipkart-interview-experience-set-39/

There are 10 houses in a row. A thief can’t pick money from the adjacent houses, how much maximum money he can make with this condition ?

*/
#include<stdio.h>

#define MAX(x, y) ((x) > (y) ? (x) :(y))

int house_thief(int arr[], int n)
{

	int max[n], curr = 0;
	int i = 0;

	max[0] = arr[0];
	max[1] = MAX(arr[0], arr[1]);

	/* note that thief can skip either 1 or 2 houses*/
	for(i = 2; i < n ; i++)
	{
		max[i] = MAX(max[i-1], max[i-2] + arr[i]);
	}

	return max[n-1];
} 

int main()
{
	int r = 0;
	int arr[] = {1, 1, 13, 10, 2, 5};
	//int arr[] = {5, 5, 10, 100, 10, 5};
	//int arr[] = {5,  5, 10, 40, 50, 35};

	printf("MAx is %d\n", house_thief(arr, sizeof(arr)/sizeof(arr[0])));
}
