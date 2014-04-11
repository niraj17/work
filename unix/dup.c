#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>


int main() {

	int fd[2], r = 0;

	r = pipe(fd);

	if (r != 0 ) {
		printf("pipe failed : %d\n", r);
		exit(r);
	}

	if (fork()) { /*parent */
		close(fd[0]);
		dup2(fd[1], STDOUT_FILENO);
		execl("/bin/ls", "ls", NULL);

	} else { /* child */
		close(fd[1]);
		dup2(fd[0], STDIN_FILENO);
		execl("/usr/bin/wc", "wc", NULL);
	}
}
