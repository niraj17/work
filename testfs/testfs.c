/*
*
* TESTFS implementation
*/

#include "testfs.h"
#include "testfs_internal.h"

#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>
#include<errno.h>


testFS testfs_connect(char *fs_path, int  flags)
{
	int ret = 0, fd = 0;
	testFS tfs = malloc (sizeof(testFS *));
	struct _testfs_superblock  *tsb = malloc (sizeof(struct _testfs_superblock));

	fd = open(fs_path, O_RDWR| O_CREAT, S_IRWXU);
	if (fd < 0) {
		perror("Error");
		exit(1);
	}
	tfs->fd = fd;

	/* if not initlized, initlize it */
	ret = read(tfs->fd, tsb, sizeof(struct _testfs_superblock ));
	printf("READ : ret=%d\n", ret);

	printf("MAgic = %s\n", tsb->magic);

	if (strcmp(tsb->magic, TESTFS_MAGIC) != 0) {

		printf("New filesystem, initlizing superblock ...\n");
		strncpy(tsb->magic, TESTFS_MAGIC, 8);
		tsb->block_size = 1024;
		tsb->total_size = 1024;
		tsb->itable_offset = 1; /* first block*/
		tsb->itable_size   = 7; /* total seven block*/
		tsb->bitmap_start  = 9;
		tsb->bitmap_size   = 2; /* should be calculated !!*/
		tsb->data_start    = 16;
		
		ret = write(tfs->fd, tsb, sizeof(struct _testfs_superblock ));
		printf("Write : ret=%d\n", ret);
	}

	printf("SB: block_size=%d, itable_size=%d, data_start = %d\n", tsb->block_size, tsb->itable_size, tsb->data_start);

	return tfs;
}

testFile testfs_open(testFS tfs, char *file_path, int flags)
{
	testFile tfile;


	return tfile;
}
void testfs_close(testFS tfs, testFile tfile)
{


	
}

int testfs_read(testFS tfs, testFile tfile, char *buffer, int size)
{

	return size;
}
int testfs_write(testFS tfs, testFile tfile, char *buffer, int size)
{



	return size;
}
