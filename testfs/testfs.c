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


static int read_block(struct _testFS *fs, int b, struct _testfs_buffer *bf)
{
	bf = malloc(sizeof(struct _testfs_buffer));
	bf->data = malloc(sizeof(fs->sb->block_size));

	lseek(fs->fd, fs->sb->block_size *b, SEEK_SET);
	read(fs->fd, bf->data, sizeof(fs->sb->block_size));
	return 0;
}
static int  get_block_number(struct _testFS *fs, struct _testfs_inode *ti, int fbn)
{
	if (fbn <= 5 ) { /* direct blocks */
		return ti->blocks[fbn];
        }  else { /* TODO : indirect blocks */
		printf("INDIRECT block not implemented \n\n");
	}
	return 0;
}
static int  create_name(struct _testFS *fs, struct _testfs_inode *parent, char *name, int len, int type)
{




}

static int find_name_in_buffer(struct _testfs_buffer *bf, char *n, int len, int limit)
{
	struct _testfs_dirent *dirent = (struct _testfs_dirent *) bf->data;
	while (limit) {
		if (strncmp(dirent->name, n, len) == 0) {
			return dirent->ino;
		}
		dirent++;
		limit = limit - sizeof (struct _testfs_dirent);
	}
	return 0;
}

static int get_name_in_inode(struct _testFS *fs, struct _testfs_inode *ti, char *name, int len, int flags)
{
	int bcount = 0, limit = 0, i = 0, ino = 0;
	int bno = 0, type = 0;
	struct _testfs_buffer *bf = NULL;

	if (ti->size > 0) {
		bcount = ti->size/fs->sb->block_size + 1;
	} 
	for (i=0; i < bcount; i++) {
		bno = get_block_number(fs, ti, i);
		read_block(fs, bno, bf);

		limit = ti->size - i* fs->sb->block_size;
		if (limit > fs->sb->block_size) limit = fs->sb->block_size;

		ino = find_name_in_buffer(bf, name, len, limit);
		if (ino) return ino;
	}
	/* if not found, and O_CREAT flag given, then create it*/
	if (flags & O_CREAT) {
		if (flags & O_DIRECTORY) {
			type = INODE_DIR;
		} else {
			type = INODE_FILE;
		}
		ino = create_name(fs, ti, name, len, type);
		if (ino) return ino;
	}
	return 0;
}
static struct _testfs_inode *read_inode(struct _testFS *tfs, int ino)
{
	struct _testfs_inode *tinode = malloc(sizeof (struct _testfs_inode ));
	lseek(tfs->fd, tfs->sb->itable_offset * tfs->sb->block_size + ino * sizeof(struct _testfs_inode), SEEK_SET);
	read(tfs->fd, tinode, sizeof(struct _testfs_inode));
	return tinode;
}

/* ==============================================================*/
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
	tfs->sb = tsb;

	printf("MAgic = %s\n", tsb->magic);

	if (strcmp(tsb->magic, TESTFS_MAGIC) != 0) {

		printf("New filesystem, initlizing superblock ...\n");
		strncpy(tsb->magic, TESTFS_MAGIC, 8);
		tsb->block_size = 1024;
		tsb->total_size = 1024;
		tsb->inode_size = INODE_SIZE;
		tsb->itable_offset = 1; /* first block*/
		tsb->itable_size   = 7; /* total seven block*/
		tsb->bitmap_start  = 9;
		tsb->bitmap_size   = 2; /* should be calculated !!*/
		tsb->data_start    = 16;
		
		ret = write(tfs->fd, tsb, sizeof(struct _testfs_superblock ));
		printf("Write : ret=%d\n", ret);

		/* create root inode */
		tfs->root = malloc(sizeof (struct _testfs_inode ));
		tfs->root->type = INODE_DIR;
		tfs->root->ino  = ROOT_INODE;
		tfs->root->size = 0;

		/* write root inode */
		lseek(tfs->fd, tsb->itable_offset * tsb->block_size + ROOT_INODE*sizeof(struct _testfs_inode), SEEK_SET);
		ret = write(tfs->fd, tfs->root, sizeof(struct _testfs_inode));
		printf("Write : ret=%d\n", ret);
	} else {
		/* read root inode */
		tfs->root = read_inode(tfs, ROOT_INODE);
	}

	printf("SB: block_size=%d, itable_size=%d, data_start = %d\n", tsb->block_size, tsb->itable_size, tsb->data_start);

	return tfs;
}

testFile testfs_open(testFS tfs, char *file_path, int flags)
{
	testFile tfile;

	struct _testfs_inode *tinode = tfs->root;
	struct _testfs_inode ti_tmp, *ti;
	int len = 0, ino = 0, myflags = flags;

	char *path = file_path;
	char buf[128], *c, *start;
	c = buf;

	while (*path != '\0') {
		while (*path == '/') path++;
		if (*path == '\0') break;

		len = 0; start = path;
		while (*path != '/' && *path != '\0') {
			path++;
			len++;
		}
		if (len == 0) break;

		if (*path == '\0') {
			/* this is the last component in the path, so must be file*/
			myflags = flags;
		} else {
			myflags = flags | O_DIRECTORY;
		}

		/* read inode and find the file */
		ti = read_inode(tfs, tinode->ino);
		/* TODO:check if it's a directory or not*/
		ino = get_name_in_inode(tfs, ti, start, len, myflags);
		tinode = read_inode(tfs, ino);
	}
	if (tinode && tinode != tfs->root) {
		tfile = malloc(sizeof (testFile  *));
		tfile->inode = tinode;
		tfile->offset = 0;
		return tfile;
	} else {
		return NULL;
	}
}
void testfs_close(testFS tfs, testFile tfile)
{


	
}

int testfs_read(testFS tfs, testFile tfile, char *buffer, int size)
{
	int bno = 0, fbn1 = 0, fbn2 = 0;
	int fbn1_start = 0, fbn2_end = 0;
	if (tfile->offset > 0) {
		fbn1 = tfile->offset/tfs->sb->block_size;
	} 
	fbn2 = (tfile->offset + size)/tfs->sb->block_size;

	bno = get_block_number(tfs, tfile->inode, fbn1);

	tfile->offset = tfile->offset + size;
	return size;
}
int testfs_write(testFS tfs, testFile tfile, char *buffer, int size)
{
	int bno = 0, fbn1 = 0, fbn2 = 0;
	int fbn1_start = 0, fbn2_end = 0;

	if (tfile->offset > 0) {
		fbn1 = tfile->offset/tfs->sb->block_size;
	} 

	bno = get_block_number(tfs, tfile->inode, fbn1);



	tfile->offset = tfile->offset + size;
	return size;
}
