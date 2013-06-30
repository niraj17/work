#include<stdio.h>

#define MAX 10

int buf[MAX];

int quick_sort(int start, int length)
{
	int k = 0, p = 0, tmp= 0;
	int j = 0;

	if (length <= 1) return;

	/* choose pivot*/
	p  = start;
	/* partition */
	j = start + 1;
	for (k=start+1; k< start+length; k++) {
		if ((buf[p] > buf[k])  ) {
			j++;
		} else {
			tmp = buf[k];
			buf[k] = buf[p];
			buf[p] = tmp;
		}
	}

	/* recurse */
	quick_sort(start,  p - start);
	quick_sort(p+1, length-p-1);
}


int main(int argc, char *argv[])
{
	int i=0;

	for (i=0; i < MAX; i++)
	{
		printf(" %d ", buf[i]);
	}
}
