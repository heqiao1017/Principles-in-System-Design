#include <errno.h>
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

#define BACKLOG 200
// if more than BACKLOG clients in the server accept queue, client connect will fail

#define N_THREAD 5
pthread_mutex_t leader_mutex;

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
        error("read from socket");
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

void *accept_client_requests(void *socket_ptr)
{
    struct sockaddr_in their_addr;
    int sin_size = sizeof( struct sockaddr_in),theirsocket;
    int mysocket = *((int *)socket_ptr);
    set_time_out(5);
    while(1)
    {
        pthread_mutex_lock(&leader_mutex);
        if((theirsocket = accept(mysocket, (struct sockaddr*)&their_addr, (socklen_t *)&sin_size))<0){
            fprintf(stderr, "-lf_server: error accepting: %s\n", strerror(errno));
            pthread_mutex_unlock(&leader_mutex);
            break;
        }
        set_time_out(5);
        
        pthread_mutex_unlock(&leader_mutex);
        handle_request(theirsocket);
    }
    
    pthread_exit(NULL);
    return NULL;
}

void create_threads(int server_socket)
{
    pthread_t threads[N_THREAD];
    pthread_mutex_init(&leader_mutex, NULL);
    
    for (int i = 0; i < N_THREAD; i++)
        pthread_create(&(threads[i]), NULL, accept_client_requests, (void *)&server_socket);
    for (int i = 0; i < N_THREAD; i++)
        pthread_join(threads[i],NULL);
}

/*
 Note, I put in a 5 second time out so you don't leave servers running.
 */

int main(int argc, char *argv[])
{
    if ( argc != 2 )
        error("usage: server port");
    int server_socket = create_server_socket(atoi(argv[1]));
    create_threads(server_socket);
    shutdown(server_socket, 2);
    pthread_mutex_destroy (&leader_mutex);
    pthread_exit(NULL);
    return 0;
}
