/*
*
* Interface for TESTFS used by client program
*/

#define TESTFS_MAGIC "TesTFs1\0"

#define INODE_FILE 0
#define INODE_DIR  1

#define ROOT_INODE 1

struct _testfs_superblock {
	char magic[8];
	int  block_size; 
	int  total_size; 
	int  inode_size; 
	int  inode_count; 
	int  itable_offset; /* offset of inode table*/
	int  itable_size;   /* size of inode table*/
	int  ibitmap_start; /* offset of inode table bitmap*/
	int  ibitmap_size;   /* size of inode table bitmap*/
	int  bitmap_start;  
	int  bitmap_size;  
	int  data_start;    /* offset of data block start */
};

struct _testfs_inode {
	int ino;
	int type; /* file or directory */
	int owner;
	int size;
	int permission;
	int dummy[3];  /* to make inode 64 byte long*/
	int blocks[8];  /* first 6 are direct, 7 is double indirect, 8 is triple indirect*/
};
#define INODE_SIZE 64

struct _testfs_dirent {
	int  ino;
	int  len; /* actual length of the stored name*/
	char name[24];
};

struct _testfs_buffer {
	int flags;
	char *data;
};


struct _testFS {
	char fs_path[1024];
	int fd;
	struct _testfs_superblock *sb;
	struct _testfs_inode *root;
};

struct _testFile {
	char file_path[256];
	int offset;
	struct _testfs_inode *inode;
};
