
all: testfs client
testfs: 
	gcc -c testfs.c -o testfs.o
client:
	gcc client.c testfs.o
clean:
	rm *.o a.out
