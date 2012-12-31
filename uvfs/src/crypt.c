/*
* Compile using:
*    gcc -o crypt crypt.c `libgcrypt-config --cflags --libs`
*
*/

#include<stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include<gcrypt.h>

int main(int argc, char *argv[])
{
    int fd, i, algo = GCRY_MD_MD5, length=0;
    unsigned char *digest;
    char buffer[4096];	
    gcry_md_hd_t hd;

    /*
    gcry_md_hash_buffer (algo, digest, buffer, length);
    */

    gcry_md_open (&hd, algo, NULL);

    if( (fd = open(argv[1], O_RDONLY)) < 0) {
	printf("Cannot open %s\n", argv[1]);
	exit (2);
    }

    while ((length = read(fd, buffer, 4096)) > 0) {
	gcry_md_write (hd, buffer, length);
    }

    digest = gcry_md_read(hd, algo);

    for (i = 0; i < gcry_md_get_algo_dlen( GCRY_MD_MD5 ); i++)
	printf("%02x", digest[i]);

    printf("\n");

}
