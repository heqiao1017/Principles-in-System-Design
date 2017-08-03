my_ls: main.o my_ls.o
		gcc -std=c99 -D_BSD_SOURCE main.o my_ls.o -o my_ls
my_ls.o: my_ls.c my_ls.h
		gcc -ggdb -std=c99 -D_BSD_SOURCE -c my_ls.c
main.o: main.c my_ls.h
		gcc -std=c99 -D_BSD_SOURCE -c main.c
clean:
		/bin/rm -f main.o my_ls.o my_ls
touch:
		my_ls.c my_ls.h main.c 
