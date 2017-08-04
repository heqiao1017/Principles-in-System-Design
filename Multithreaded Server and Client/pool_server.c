#include <sys/types.h>
#include <signal.h>
#include <sys/uio.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <pthread.h>
#include <string.h>
#define N_THREADS 5
#define BACKLOG 200
// if more than BACKLOG clients in the server accept queue, client connect will fail

int	q[300];						// queue to hold produced numbers
int	first  = 0;					// index into q of next free space
int last   = 0;					// index into q of next available number
int	numInQ = 0;					// number of items in the queue
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;	// mutual exclusion lock
pthread_mutex_t empty = PTHREAD_MUTEX_INITIALIZER;	// synchronization lock
pthread_mutex_t full = PTHREAD_MUTEX_INITIALIZER;
/* putOnQ is called by the producer threads to put a number on the queue */
void putOnQ(int x) {
    while (numInQ == 300) pthread_mutex_lock(&full);
    pthread_mutex_lock(&mutex);		// lock access to the queue
    q[first] = x;					// put item on the queue
    first = (first+1) % 300;
    numInQ++;						// increment queue size
    pthread_mutex_unlock(&mutex);	// unlock queue
    pthread_mutex_unlock(&empty);	// start a waiting consumer
}

/* getOffQ is called by consumer threads to retrieve a number
 from the queue.  The thread will be suspended if there is
 nothing on the queue. */
int getOffQ() {
    int thing;						// item removed from the queue
    /* wait if the queue is empty. */
    while (numInQ == 0) pthread_mutex_lock(&empty);
    pthread_mutex_lock(&mutex);		// lock access to the queue
    thing = q[last];				// get item from the queue
    last = (last+1) % 300;
    numInQ--;						// decrement queue size
    pthread_mutex_unlock(&full);
    pthread_mutex_unlock(&mutex);	// unlock queue
    return thing;
}


void error(char *msg)
{
    fprintf(stderr, "ERROR: %s failed\n", msg);
    exit(-1);
}


struct sockaddr_in make_server_addr(short port)
{
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    return addr;
}

int create_server_socket(short port)
{
    int s = socket(AF_INET, SOCK_STREAM, 0);
    int optval = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);
    struct sockaddr_in my_addr = make_server_addr(port);
    if ( s == -1 )
        error("socket()");
    bind(s, (struct sockaddr*)&my_addr, sizeof my_addr);
    listen(s, BACKLOG);
    return s;
}

void get_file_request(int socket, char *fileName)
{
    char buf[BUFSIZ];
    int n = read(socket, buf, BUFSIZ);
    if ( n < 0 )
        perror("read from socket");
    buf[n] = '\0';
    strcpy(fileName, buf);
    printf("Server got file name of '%s'\n", fileName);
}

void write_file_to_client_socket(char *file, int socket)
{
    char buf[BUFSIZ];
    int n;
    int ifd = open(file, O_RDONLY);
    if ( ifd == -1 )
        error("open()");
    while ( (n = read(ifd, buf, BUFSIZ)) > 0 )
        write(socket, buf, n);
    close(ifd);
}

void handle_request(int client_socket)
{
    char fileName[BUFSIZ];
    get_file_request(client_socket, fileName);
    write_file_to_client_socket(fileName, client_socket);
    close(client_socket);
}

void time_out(int arg)
{
    fprintf(stderr,  "Server timed out\n");
    exit(0);
}

void set_time_out(int seconds)
{
    struct itimerval value = {0};
    value.it_value.tv_sec = seconds;
    setitimer(ITIMER_REAL, &value, NULL);
    signal(SIGALRM, time_out);
}

void accept_client_requests(int server_socket)
{
    int client_socket;
    struct sockaddr_in client_addr;
    socklen_t sin_size = sizeof client_addr;
    set_time_out(5);
    while (1)
    {
        set_time_out(5);
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &sin_size);
        putOnQ(client_socket);
    }
}


void *thread_work(void *arg)
{
    while(1)
    {
        handle_request(getOffQ());
    }
}


/*
 Note, I put in a 5 second time out so you don't leave servers running.
 */

int main(int argc, char *argv[])
{
    if ( argc != 2 )
        error("usage: server port");
    
    pthread_t threads[N_THREADS];
    for ( int i = 0; i < N_THREADS; ++i )
    {
        pthread_create(&threads[i], NULL, thread_work, NULL);
    }
    
    short port = atoi(argv[1]);
    int server_socket = create_server_socket(port);
    accept_client_requests(server_socket);
    shutdown(server_socket, 2);
    return 0;
}
