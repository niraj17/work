# 
# UVFS - Userspace Version File System
#
UVFS_VERSION="\"0.10\""

FUSE_FLAGS=`pkg-config fuse --cflags --libs`
GCRYPT_FLAGS=`libgcrypt-config --cflags --libs`
CFLAGS = $(shell mysql_config --cflags)

all: uvfs auditlib test
uvfs: uvfs.c hashtable.c
	gcc -Wall -c hashtable.c
	gcc -Wall -c ${FUSE_FLAGS} -DUVFS_VERSION=${UVFS_VERSION} uvfs.c
	gcc -Wall ${FUSE_FLAGS} uvfs.o hashtable.o -o uvfs

auditlib: auditlib.c  auditlib.h
	gcc -shared auditlib.c -o auditlib.so ${GCRYPT_FLAGS}
test:  test.c
	cc $(CFLAGS) test.c hashtable.c -ldl -ldb -o test -ldblayer -Ldblayer
clean:
	rm uvfs test *.so  *.o  a.out
