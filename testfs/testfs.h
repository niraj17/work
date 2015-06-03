/*
*
* Interface for TESTFS used by client program
*/

#define TESTFS_CREATE  0x1

typedef struct _testFS * testFS;

typedef struct _testFile * testFile;

testFS testfs_connect(char *fs_path, unsigned int  flags);

testFile testfs_open(testFS , char *file_path,unsigned  int flags);
void testfs_close(testFS , testFile);

int testfs_read(testFS , testFile , char *buffer, int size);
int testfs_write(testFS , testFile , char *buffer, int size);
