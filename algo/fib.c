#include<stdio.h>

int mem[20];

int fib(int n)
{
	int k = 0;
	if (n < 2) return n;
	if (mem[n-1] == 0)
		mem[n-1] = fib(n-1);
	if (mem[n-2] == 0)
		mem[n-2] = fib(n-2);

	return mem[n-1] + mem[n-2];
}


int main(int argc, char *argv[])
{
	int i=0;
	for (i=0; i < 7; i++)
	{
		printf(" %d ", fib(i));
	}
}
