
/* client program for testfs filesystem */

#include<stdio.h>
#include "testfs.h"


int main()
{
	int ret = 0;

	testFS  tfs = testfs_connect("myfs", 0);

	testFile tfile = testfs_open(tfs, "/a/b", 0);
	ret  = testfs_write( tfs, tfile, "Hello world!", 12 );

	return 0;
}
