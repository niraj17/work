/*
 * Program for running scripts with root permission.
 * 
 * You need to "install" this as root (one time operation).
 *
 * After that, normal user can run scripts with root permission
 *  ./run_root <your_program> <your_program_arguments>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
   char command[2048];
   int length = 0, i = 0;

   if (argc <= 1) {
	printf("Usage : %s <arguments>\n", argv[0]);
	exit(1);
   } 

   for(i=1; i < argc; i++) {
     length += sprintf(command+length, " %s", argv[i]);
   }
   //printf("command = %s\n", command);

   // become root
   setuid(0);
	
   return system(command);
}
