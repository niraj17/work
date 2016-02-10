/*
 * See http://www.geeksforgeeks.org/flipkart-interview-experience-set-28-for-sde2/
 *     http://www.careercup.com/question?id=12658675
 */
#include<stdio.h>
#include<stdlib.h>


water_collected(int *list, int size)
{
	int i =  0, water = 0, min = 0;
	int *max_l = malloc(sizeof(int) * size);
	int *max_r = malloc(sizeof(int) * size);

	for(i=1; i < size; i++)
		max_l[i] = (list[i-1] > max_l[i] ? list[i-1] : max_l[i]);
	for(i=size-1; i >= 0; i--)
		max_r[i-1] = (list[i] > max_r[i-1] ? list[i] : max_r[i-1]);

	for(i=1; i < size- 1; i++) {
		min = (max_r[i] > max_l[i] ? max_l[i] : max_r[i]); 
		if (min < list[i]) continue;
		water += min - list[i]; 
	}

	return water;

}

int main()

{
	int w =  0;
	int a[] = {3, 0, 2};
	int b[] = {1,5,3,7,2};

	w = water_collected(b, 5);
	printf("Water is %d\n", w);
}
