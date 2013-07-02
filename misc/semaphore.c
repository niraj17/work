#include<fcntl.h>           /* For O_* constants */
#include<sys/stat.h>        /* For mode constants */
#include<semaphore.h>
#include<stdlib.h>
#include<stdio.h>
#include<errno.h>

int main(int argc, char *argv[])
{
	sem_t *sem = sem_open("/mysem", O_CREAT| O_EXCL , O_RDWR, 1);
	
	if (sem == SEM_FAILED) {
		perror("error:");
		exit(1);
	}
}
