#include<stdio.h>

int main()
{
	int a= 10, b= 20, diff;

	diff = &b - &a;

	printf("diff is %d\n",diff);
}
