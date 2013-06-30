#include<stdio.h>


char buf[20];


char *itoa(int n)
{
	int i= 0, k = 0, r, j;
	char tmp[20];

	while (1) {
		r = n% 10;

		tmp[i] = '0' + r;
		i++;

		n = n / 10;
		if (n <= 0) break;
	}
	for (j=i-1; j >= 0; j--) {
		buf[k] = tmp[j];
		k++;
	}
	buf[k] = 0;
	return buf;
}




int main(int argc, char *argv[])
{
	char *s = itoa(236);
	printf("String = %s\n", s);

	int a[17];
	int k = &a[14] - &a[5];
	printf("K = %d\n", k);
}
