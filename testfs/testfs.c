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


static int alloc_empty_slot(char *b, int size)
{
	unsigned int  mask = 1;
	int j, k ;
	int *data = (int *) b;

	for (k=0; k < size/32; k++) {
		for (j=0; j < 32; j++) {
			if (!(*data & mask)) {
				*data = *data | mask;
				return j + 32*k;
			}
			mask = mask << 1;
		}
		data++;
		mask = 1;
	}
	return -1;
}
static int write_block(struct _testFS *fs, int bno, struct _testfs_buffer *bf)
{
	lseek(fs->fd, fs->sb->block_size * bno, SEEK_SET);
	write(fs->fd, bf->data, sizeof(fs->sb->block_size));
	return 0;
}
static int read_block(struct _testFS *fs, int bno, struct _testfs_buffer **bf)
{
	struct _testfs_buffer *b = NULL;
	b = malloc(sizeof(struct _testfs_buffer));
	b->data = malloc(sizeof(fs->sb->block_size));

	lseek(fs->fd, fs->sb->block_size * bno, SEEK_SET);
	read(fs->fd, b->data, sizeof(fs->sb->block_size));

	*bf = b;
	return 0;
}
static int free_buffer(struct _testfs_buffer *bf) 
{
	free(bf->data);
	free(bf);
}
static struct _testfs_inode *read_inode(struct _testFS *tfs, int ino)
{
	struct _testfs_inode *tinode = malloc(sizeof (struct _testfs_inode ));
	lseek(tfs->fd, tfs->sb->itable_start * tfs->sb->block_size + ino * sizeof(struct _testfs_inode), SEEK_SET);
	read(tfs->fd, tinode, sizeof(struct _testfs_inode));
	return tinode;
}
static int write_inode(struct _testFS *tfs, struct _testfs_inode *ti)
{
		/* write inode */
		lseek(tfs->fd, tfs->sb->itable_start * tfs->sb->block_size + ti->ino *sizeof(struct _testfs_inode), SEEK_SET);
		write(tfs->fd, ti, sizeof(struct _testfs_inode));
}
int alloc_block(struct _testFS *fs, struct _testfs_inode *ti, int fbn)
{
	struct _testfs_buffer *bf = NULL, *bf1 = NULL;
	int bno = fs->sb->bitmap_start, new_bno = 0;
	int i = 0, slot = 0, ino = 0, count = 0;
	for (i=bno; i< bno+fs->sb->bitmap_size; i++)
	{
		read_block(fs, i, &bf);
		slot = alloc_empty_slot(bf->data, fs->sb->block_size);
		if (slot >= 0) {

			write_block(fs, i, bf);
			free_buffer(bf);
			new_bno = fs->sb->data_start + slot + count * fs->sb->block_size * 8;
			/* update pointer in inode*/
			ti->blocks[fbn] = new_bno; 
			return new_bno;
		}
		free_buffer(bf);
		count++;
	}
	return 0;
}

static int  get_block_number(struct _testFS *fs, struct _testfs_inode *ti, int fbn)
{
	if (fbn <= 5 ) { /* direct blocks */
		return ti->blocks[fbn];
        }  else { /* TODO : indirect blocks */
		printf("INDIRECT block not implemented \n\n");
	}
	return -1; /* not allocated yet*/
}
static int  create_name(struct _testFS *fs, struct _testfs_inode *parent, char *name, int len, int type)
{
	struct _testfs_inode *child = malloc(sizeof (struct _testfs_inode));
	struct _testfs_buffer *bf = NULL, *bf1 = NULL;
	struct _testfs_dirent *de = malloc (sizeof (struct _testfs_dirent));
	int slot = -1, i = 0, ino = 0, count = 0, offset = 0;
	/* allocate a new inode on disk and write it */
	/*child_inode = alloc_inode(); */
	int bno = fs->sb->ibitmap_start;

	for (i=bno; i< bno+fs->sb->ibitmap_size; i++)
	{
		read_block(fs, i, &bf);
		slot = alloc_empty_slot(bf->data, fs->sb->block_size);
		if (slot >= 0) {
			ino = slot + count * fs->sb->block_size * 8;
			child->ino = ino;
			child->type = type;
			child->size = 0;

			write_block(fs, i, bf);
			write_inode(fs, child);

			free_buffer(bf);
			break;
		}
		free_buffer(bf);
		count++;
	}

	/* create a new dirent in the parent for the child*/
	de->ino = ino;
	memcpy(de->name, name, len);
	de->len = len;

	bno = parent->size/fs->sb->block_size;
	offset = parent->size - bno * fs->sb->block_size;
	read_block(fs, bno, &bf1);
	memcpy(bf1->data+offset, de, sizeof(struct _testfs_dirent));
	write_block(fs, bno, bf1);
	free_buffer(bf1);

	parent->size += sizeof (struct _testfs_dirent);
	write_inode(fs, parent);

	return 0;

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

static int get_name_in_inode(struct _testFS *fs, struct _testfs_inode *ti, char *name, int len, unsigned int flags)
{
	int bcount = 0, limit = 0, i = 0, ino = 0;
	int bno = 0, type = 0;
	struct _testfs_buffer *bf = NULL;

	if (ti->size > 0) {
		bcount = ti->size/fs->sb->block_size + 1;
	} 
	for (i=0; i < bcount; i++) {
		bno = get_block_number(fs, ti, i);
		read_block(fs, bno, &bf);

		limit = ti->size - i* fs->sb->block_size;
		if (limit > fs->sb->block_size) limit = fs->sb->block_size;

		ino = find_name_in_buffer(bf, name, len, limit);
		free_buffer(bf);
		if (ino) return ino;
	}
	/* if not found, and O_CREAT flag given, then create it*/
	if (flags & TESTFS_CREATE) {
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
static int init_bitmaps(struct _testFS *fs) /* also allocates root inode*/
{
	int bno = 0, i = 0;
	int *data = NULL;
	struct _testfs_buffer *bf = NULL;

	read_block(fs, fs->sb->ibitmap_start, &bf);
	bzero(bf->data, sizeof(fs->sb->block_size));
	data  = (int *) bf->data;
	*data = 0xCFFFFFFF; /* Allocate first two bits*/
	write_block(fs, fs->sb->ibitmap_start, bf);
	free_buffer(bf);

	for (bno = fs->sb->ibitmap_start+1; bno < fs->sb->data_start-1;
			bno++)
	{
		read_block(fs, fs->sb->ibitmap_start, &bf);
		bzero(bf->data, sizeof(fs->sb->block_size));
		write_block(fs, fs->sb->ibitmap_start, bf);
	}
}

/* ==============================================================*/
testFS testfs_connect(char *fs_path, unsigned int  flags)
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
		tsb->total_size = 1024; /* in terms of blocks*/
		tsb->inode_size = INODE_SIZE;
		tsb->inode_count= 256;

		tsb->itable_start = 2;
		tsb->itable_size   = tsb->inode_count/(tsb->block_size/INODE_SIZE);
		tsb->ibitmap_start = tsb->itable_start + tsb->itable_size + 1;
		tsb->ibitmap_size  = tsb->inode_count/tsb->block_size +1;
		tsb->bitmap_start  = tsb->ibitmap_start + tsb->ibitmap_size + 1;
		tsb->bitmap_size   = tsb->total_size/tsb->block_size + 1; /* should be calculated !!*/
		tsb->data_start    = tsb->bitmap_start + tsb->bitmap_size + 1;
		
		ret = write(tfs->fd, tsb, sizeof(struct _testfs_superblock ));
		printf("Write : ret=%d\n", ret);


		/* create root inode */
		tfs->root = malloc(sizeof (struct _testfs_inode ));
		tfs->root->type = INODE_DIR;
		tfs->root->ino  = ROOT_INODE;
		tfs->root->size = 0;

		init_bitmaps(tfs); /*Also allocates root inode */
		/* write root inode */
		write_inode(tfs, tfs->root);
	} else {
		/* read root inode */
		tfs->root = read_inode(tfs, ROOT_INODE);
	}

	printf("SB: block_size=%d, itable_size=%d, data_start = %d\n", tsb->block_size, tsb->itable_size, tsb->data_start);

	return tfs;
}

/* There is no seperate create call. open with TESTFS_CREATE flag will
 * create the file, including intermediate directories, if needed.
 */
testFile testfs_open(testFS tfs, char *file_path, unsigned int flags)
{
	testFile tfile;

	struct _testfs_inode *tinode = tfs->root;
	struct _testfs_inode ti_tmp, *ti;
	int len = 0, ino = 0;
	unsigned int myflags = flags;

	char *path = file_path;
	char  *c, *start;

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
		if (ino == 0) break;
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
	tfile->offset = 0;
	free(tfile);
}

int testfs_read(testFS tfs, testFile tfile, char *buffer, int size)
{
	int bno = 0, fbn1 = 0, fbn2 = 0; /* FBNs starts from zero*/
	int fbn1_start = 0, fbn2_end = 0, i = 0;
	int remaining_size = size, b_len = 0;
	struct _testfs_buffer *bf = NULL;
	char *out = buffer;

	if (tfile->offset > 0) {
		fbn1 = tfile->offset/tfs->sb->block_size;
		fbn1_start = tfile->offset - fbn1 * tfs->sb->block_size;
	}
	fbn2 = (tfile->offset + size)/tfs->sb->block_size;

	b_len = tfs->sb->block_size - fbn1_start;

	/* read first block */
	bno = get_block_number(tfs, tfile->inode, fbn1);
	read_block(tfs, bno, &bf);
	memcpy(out, bf->data + fbn1_start, b_len);
	free_buffer(bf);
	out = out + b_len;
	remaining_size = remaining_size - b_len;

	/* TODO : check for END-OF-File case !!!*/

	if (fbn2 > fbn1 ) {
		/* read intermediate blocks */
		for (i=fbn1+1; i<fbn2-1; i++) {
			bno = get_block_number(tfs, tfile->inode, i);
			read_block(tfs, bno, &bf);
			memcpy(out, bf->data , tfs->sb->block_size);
			free_buffer(bf);
			out = out + tfs->sb->block_size;
			remaining_size = remaining_size - tfs->sb->block_size;
		}
		/* read last block */
		bno = get_block_number(tfs, tfile->inode, fbn2);
		read_block(tfs, bno, &bf);
		memcpy(out, bf->data, remaining_size);
		free_buffer(bf);
	}

	tfile->offset = tfile->offset + size;
	return size;
}
int testfs_write(testFS tfs, testFile tfile, char *buffer, int size)
{
	int bno = 0, fbn1 = 0, fbn2 = 0; /* FBNs starts from zero*/
	int fbn1_start = 0, fbn2_end = 0, i = 0;
	int remaining_size = size, b_len = 0;
	struct _testfs_buffer *bf = NULL;
	char *out = buffer;
	struct _testfs_buffer *bf_new = malloc(sizeof(struct _testfs_buffer));
	bf_new->data = malloc(sizeof(tfs->sb->block_size));

	if (tfile->offset > 0) {
		fbn1 = tfile->offset/tfs->sb->block_size;
		fbn1_start = tfile->offset - fbn1 * tfs->sb->block_size;
	}
	fbn2 = (tfile->offset + size)/tfs->sb->block_size;

	b_len = tfs->sb->block_size - fbn1_start;
	if (remaining_size < b_len)  b_len = remaining_size;

	/* alloc/write first block */
	bno = get_block_number(tfs, tfile->inode, fbn1);
	if (bno > 0) {
		read_block(tfs, bno, &bf);
		memcpy(bf->data + fbn1_start, out, b_len);
		write_block(tfs, bno, bf);
		free_buffer(bf);
	} else {
		bno = alloc_block(tfs, tfile->inode, fbn1);
		memcpy(bf_new->data + fbn1_start, out, b_len);
		write_block(tfs, bno, bf_new);
	}
	out = out + b_len;
	remaining_size = remaining_size - b_len;

	if (fbn2 > fbn1 ) {
		/* alloc/write intermediate blocks */
		for (i=fbn1+1; i<fbn2-1; i++) {
			bno = get_block_number(tfs, tfile->inode, i);
			if (bno > 0) {
				read_block(tfs, bno, &bf);
				memcpy(bf->data , out, tfs->sb->block_size);
				write_block(tfs, bno, bf);
				free_buffer(bf);
			} else {
				bno = alloc_block(tfs, tfile->inode, fbn1);
				memcpy(bf_new->data , out, tfs->sb->block_size);
				write_block(tfs, bno, bf_new);
			}

			out = out + tfs->sb->block_size;
			remaining_size = remaining_size - tfs->sb->block_size;
		}
		/* alloc/write last block */
		bno = get_block_number(tfs, tfile->inode, fbn2);
		if (bno > 0) {
			read_block(tfs, bno, &bf);
			memcpy(bf->data , out, remaining_size);
			write_block(tfs, bno, bf);
			free_buffer(bf);
		} else {
			bno = alloc_block(tfs, tfile->inode, fbn2);
			bzero(bf_new->data, tfs->sb->block_size);
			memcpy(bf_new->data , out, remaining_size);
			write_block(tfs, bno, bf_new);
		}
	}

	free_buffer(bf_new);

	/* update inode size*/
	if (tfile->inode->size < tfile->offset + size) {
		tfile->inode->size = tfile->offset + size;
	}
	write_inode(tfs, tfile->inode);

	tfile->offset = tfile->offset + size;
	return size;
}
