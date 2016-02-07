
/* client program for testfs filesystem */

#include<stdio.h>
#include<stdlib.h>
#include "testfs.h"


int main()
{
	int ret = 0;
	char  buffer[1024];

	testFS  tfs = testfs_connect("myfs", 0);

	testFile tfile = testfs_open(tfs, "/file1", TESTFS_CREATE);
	if (tfile == NULL) {
		printf("Failed to open file\n");
		exit(1);
	}
	ret  = testfs_write( tfs, tfile, "Write to file a!", 17 );
	testfs_close(tfs, tfile);
	printf("Write done, count=%d, ...now going to read\n", ret);

	tfile = testfs_open(tfs, "/file1", 0);
	if (tfile == NULL) {
		printf("Failed to open file\n");
		exit(1);
	}
	ret  = testfs_read(tfs, tfile, buffer, 1024);

	printf("Read count=%d, data = %s\n",ret,  buffer);

	return 0;
}
