all: concurrent_server concurrent_client ls_server ls_client
object1=concurrent_server.o
object2=concurrent_client.o
object3=ls_server.o my_ls.o
object4=ls_client.o
CC = gcc
CFLAG = -ggdb
STD= -std=c99
concurrent_server: $(object1)
	$(CC) $(CFLAG) $(STD) $(object1) -o concurrent_server
concurrent_server.o: concurrent_server.c
	$(CC) $(CFLAG) $(STD) -c concurrent_server.c
concurrent_client: $(object2)
	$(CC) $(CFLAG) $(STD) $(object2) -o concurrent_client
concurrent_client.o: concurrent_client.c
	$(CC) $(CFLAG) $(STD) -c concurrent_client.c
ls_server: $(object3)
	$(CC) $(CFLAG) $(STD) -D_BSD_SOURCE $(object3) -o ls_server
my_ls.o: my_ls.c my_ls.h
	$(CC) $(CFLAG) $(STD) -D_BSD_SOURCE -c my_ls.c
ls_server.o: ls_server.c my_ls.h
	$(CC) $(CFLAG) $(STD) -D_BSD_SOURCE -c ls_server.c
ls_client: $(object4)
	$(CC) $(CFLAG) $(STD) $(object4) -o ls_client
ls_client.o: ls_client.c
	$(CC) $(CFLAG) $(STD) -c ls_client.c
clean:
	/bin/rm -f $(object1) concurrent_server $(object2) concurrent_client $(object3) ls_server $(object4) ls_client
strip:
	strip -g -S -d concurrent_server concurrent_client ls_server ls_client

