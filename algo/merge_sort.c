#include<stdio.h>

int merge(int buf[], int t[], int start, int mid, int end)
{
	int i, j = 0, k=start;
	int count = 0;

	i = start;
	j = mid;
	while (1) {
		if (i == mid || j == end+1) break;

		if (buf[i] < buf[j]) {
			t[k] = buf[i];
			i++;
		} else {
			t[k] = buf[j];
			j++;
			count += (mid - i);
		}
		k++;
	}

	// copy the remaining items
	if (i <= mid - 1) {
		for( ; i< mid; k++, i++)
			t[k] = buf[i];
	}
	if (j<= end) {
		for(; j<=end; k++, j++)
			t[k] = buf[j];
	}

	// copy array back to the original
	for(i=start; i <= end; i++)
		buf[i] = t[i];

	return count;
}

int merge_sort(int buf[], int t[], int start, int end)
{
	int k = 0, p = 0, count=0;
	int mid = (start + end)/2;

	if (end <= start) return 0;

	printf("Merging between %d , %d, and %d\n", start,mid,  end);
	/* recurse */
	count = merge_sort(buf, t, start,  mid);
	count += merge_sort(buf, t, mid+1, end);

	/* now merge */
	count += merge(buf, t, start, mid+1, end);

	return count;
}


int main(int argc, char *argv[])
{
	int i=0 , count = 0;
	int buf[] = {2,3,6,4,1,8,9,12,11,10};
	int t[10] = {0};
	int size = sizeof(buf)/sizeof(buf[0]);

	count = merge_sort(buf, t,  0, size -1);

	printf("Inversion Count = %d\n", count);

	for(i=0; i < size; i++)
		printf("%d, ", buf[i]);

	printf("\n");

}
