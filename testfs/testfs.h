/*
*
* Interface for TESTFS used by client program
*/


typedef struct _testFS * testFS;

typedef struct _testFile * testFile;

testFS testfs_connect(char *fs_path, int  flags);

testFile testfs_open(testFS , char *file_path, int flags);
void testfs_close(testFS , testFile);

int testfs_read(testFS , testFile , char *buffer, int size);
int testfs_write(testFS , testFile , char *buffer, int size);
