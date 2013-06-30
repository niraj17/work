
/*
 *
 * Longest incresing sequence - uses Dynamic Programming 
 */

#include<stdio.h>

#define MAX 20
int buf[MAX] = {1,4,5,2,10,6,7,8,3,12,13,10,9,8,7,6,5,1,9,0};


int main(int argc, char *argv[])
{
	int i=0,j=0,k=0;
	int lis[MAX] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
	int final = 0, index  = 0;

	for (i=0; i < MAX; i++){
		for (j=0; j < i; j++){
			if (buf[i] > buf[j] && lis[i] < lis[j]+1) {
				lis[i] = lis[j] + 1;
			}
		}
	}
	for (i=0; i < MAX; i++){
		//printf("lis[%d] = %d\n", i, lis[i]);
		if (lis[i] > final) {
			final = lis[i];
			index = i;
		}
	}
	printf("LIS is %d ending at index %d\n",final,  index);
}
