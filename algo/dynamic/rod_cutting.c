/*
http://www.geeksforgeeks.org/dynamic-programming-set-13-cutting-a-rod/

*/
#include<stdio.h>

#define MAX(a,b) ((a) > (b) ? (a) : (b))

int rod_cutting_DP(int length, int cost[])
{
	int value[length+1];
	int i = 0, max, j;

	for(i=1; i <= length; i++)
	{
		for(j=1; j < i ; j++) {
			max = MAX(max, cost[j]+value[i-j-1]);
		}
		value[i] = max;
	}
	return value[length];
}


int main()
{
	int cost[] = {1,5,8,9,10,17,17,20};

	int r = rod_cutting_DP(8, cost);
	printf("R is %d\n", r);
}

