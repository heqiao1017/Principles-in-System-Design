all: product my_shell
object1=product.o
object2= my_shell.o
CC = gcc
CFLAG = -ggdb
STD= -std=c99
product: $(object1)
	$(CC) $(CFLAG) $(STD) -o product $(object1)
product.o:product.c
	$(CC) $(CFLAG) $(STD) -c product.c
my_shell: $(object2)
	$(CC) $(CFLAG) $(STD) -o my_shell $(object2)
my_shell.o: my_shell.c
	$(CC) $(CFLAG) $(STD) -c my_shell.c
clean:
	/bin/rm -f $(object1) product $(object2) my_shell
strip:
	strip -g -S -d product my_shell
