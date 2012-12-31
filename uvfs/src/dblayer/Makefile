CONFIG_FLAGS=$(shell mysql_config --cflags)
LIB_FLAGS=$(shell mysql_config --libs)

main.o : libdblayer.so main.c
	gcc -o $@  main.c $(CONFIG_FLAGS) $(LIB_FLAGS)  -L. -ldblayer
    
libdblayer.so : dblayer.o dbcommon.o
	gcc -shared -o $@  $^ $(LIB_FLAGS)
	
%.o : %.c
	gcc  -Wall -c $? $(CONFIG_FLAGS)


clean : 
	rm -rf *.o
	rm -rf main
	rm -rf *.*~$
