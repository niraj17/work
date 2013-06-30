#include<stdio.h>

#define MAX 10

int buf[MAX] = {2,3,6,4,1,8,9,12,11,10};

int merge_sort(int start, int end)
{
	int k = 0, p = 0, tmp= 0;
	int j = 0;
	int tmp[MAX];

	if (end - start <= 1) return;
	n = start + (end-start)/2;

	/* recurse */
	merge_sort(start,  n);
	merge_sort(start+n, end);

	/* now merge */
	i = start;
	j = start+n;
	while (1) {
		if (i == n && j == end) break;

		if (buf[i] < buf[j]) {
			tmp[k] = buf[i];
			i++;
		} else {
			tmp[k] = buf[j];
			j++;
		}
		k++;
	}
}


int main(int argc, char *argv[])
{
	int i=0;

	for (i=0; i < MAX; i++)
	{
		printf(" %d ", buf[i]);
	}
}
