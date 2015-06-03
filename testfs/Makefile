
all: testfs client
testfs: 
	gcc -g -Wall -c testfs.c -o testfs.o
client:
	gcc -g client.c testfs.o
clean:
	rm *.o a.out
