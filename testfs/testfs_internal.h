/*
*
* Interface for TESTFS used by client program
*/

#define TESTFS_MAGIC "TesTFs1\0"

struct _testfs_superblock {
	char magic[8];
	int  block_size; 
	int  total_size; 
	int  itable_offset; /* offset of inode table*/
	int  itable_size;   /* Number of entries in inode table*/
	int  bitmap_start;  
	int  bitmap_size;  
	int  data_start;    /* offset of data block start */
};

struct _testfs_inode {
	int owner;
	int permission;
	int blocks[8];  /* first 6 are direct, 7 is double indirect, 8 is triple indirect*/
};


struct _testFS {
	char fs_path[1024];
	int fd;
	struct _testfs_superblock *sb;
};

struct _testFile {
	char file_path[256];
	int offset;
	struct _testfs_inode *inode;
};
