#include<stdio.h>

int main()
{
	int i, n = 4;
        for(i=0; i < n;i++)
	{
		if (fork() == 0)
			printf("hello \n");
		else {
			wait();
			printf("wait \n");
		}
	}
	return 0;
}
