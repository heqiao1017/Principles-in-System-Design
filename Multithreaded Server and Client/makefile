PORT = 26777
all = per_request_server leader_follower_server pool_server client
S = -std=c99 -ggdb

per_request_server: per_request_server.c
	gcc $S per_request_server.c -lpthread -o per_request_server
leader_follower_server: leader_follower_server.c
	gcc $S leader_follower_server.c -lpthread -o leader_follower_server
pool_server: pool_server.c 
	gcc $S pool_server.c -lpthread -o pool_server
client: client.o Timer.o
	gcc $S client.o Timer.o -lpthread -o client
Timer.o: Timer.c Timer.h
	gcc -c $S Timer.c
client.o: client.c Timer.h
	gcc $S -c client.c -lpthread
	
test1: per_request_server client
	per_request_server $(PORT) &
	echo Starting client
	client `hostname` $(PORT)
	ls -lr Thread_*
	du

test2: pool_server client
	pool_server $(PORT) &
	echo Starting client
	client `hostname` $(PORT)
	ls -lr Thread_*
	du

test3: leader_follower_server client
	leader_follower_server $(PORT) &
	echo Starting client
	client `hostname` $(PORT)
	ls -lr Thread_*
	du


clean:
	/bin/rm -rf $(all) Thread_*  *.o
