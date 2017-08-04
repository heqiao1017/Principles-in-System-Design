#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <errno.h>
#include "my_ls.h"
#include <sys/wait.h>

//#define PORT 65414
#define BACKLOG 20
#define MAXMSG  512

int make_socket (uint16_t port)
{
    int sockfd;
    struct sockaddr_in my_addr;
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1)
    {
        perror("socket() error lol!");
        exit(EXIT_FAILURE);
    }
    else
        printf("socket() is OK...\n");
    
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(port);
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    memset(&(my_addr.sin_zero), 0, 8);
    
    
    if(bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("bind() error lol!");
        exit(EXIT_FAILURE);
    }
    else
        printf("bind() is OK...\n");
    
    if(listen(sockfd, BACKLOG) == -1)
    {
        perror("listen() error lol!");
        exit(EXIT_FAILURE);
    }
    else
        printf("listen() is OK...\n");
    
    
    return sockfd;
}



int process_my_ls(int new_fd,char *buffer)
{
    int nbytes;
    if ((nbytes=read(new_fd, buffer, BUFSIZ)) > 0)
    {
        switch (fork()) {
            case -1:
                perror("fork failed\n");
                exit(EXIT_FAILURE);
            case 0:
                dup2(new_fd, 1);
                do_ls(buffer);
                exit(0);
            default:
                wait(0);
                close(new_fd);
                break;
        }
        return 1;
    }
    else if(nbytes==0)
        return -1;
    else{
        perror ("read");
        exit (EXIT_FAILURE);
    }
}


int main(int argc, char** argv)
{
    if(argc != 2)
        exit(EXIT_FAILURE);
    uint16_t port = atoi(argv[1]);
    int sockfd = make_socket(port);
    int new_fd,sin_size;
    struct sockaddr_in their_addr;
    
    sin_size = sizeof (struct sockaddr_in);
    
    while(1)
    {
        if ((new_fd = accept(sockfd, (struct sockaddr *) &their_addr, &sin_size) ) < 0){
            perror("accept() error lol!");
            exit(EXIT_FAILURE);
        }
        else
            printf("accept() is OK...\n");
        
        char buffer[BUFSIZ];
        memset(buffer, 0, BUFSIZ);
        process_my_ls(new_fd,buffer);
        close(new_fd);
        printf("\n\n=================================================================\n\n");
    }
    shutdown(sockfd, 2);
    return 0;
}
