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
#include <sys/wait.h>

#define BACKLOG 20

int make_server_socket (uint16_t port)
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

void copy_file_to_socket(char *filename, int client)
{
    int file_open,read_in;
    char buff[BUFSIZ];
    memset(buff, 0, BUFSIZ);
    
    //open file
    if((file_open= open(filename, O_RDONLY)) <0){
        fprintf(stderr, "Cannot open %s to read\n",filename);
        exit(EXIT_FAILURE);
    }
    
    //read and write to socket
    while((read_in=read(file_open,buff,BUFSIZ))>0){
        if(write(client,buff,read_in)<0){
            perror("Write to client");
            exit(EXIT_FAILURE);
        }
    }
    close(file_open);
}

int read_from_client(int fd)
{
    char buffer[BUFSIZ];
    memset(buffer, 0, BUFSIZ);
    int nbytes;
    
    nbytes = read (fd, buffer, BUFSIZ);
    if (nbytes < 0)
    {
        perror ("read");
        exit (EXIT_FAILURE);
    }
    else if (nbytes == 0)
        return -1;
    else
    {
        copy_file_to_socket(buffer,fd);
        return 1;
    }
}


void sigchld_handler(int sig)
{
    while (waitpid(-1, 0, WNOHANG) > 0);
    return;
}

void fork_child_copying_file_to_socket(int sockfd,int new_fd)
{
    int pid;
    switch(pid=fork()){
        case -1:
            perror("Fork failed\n");
            exit(EXIT_FAILURE);
        case 0:
            //child
            close(sockfd);
            while(read_from_client(new_fd)>0)
                ;
            exit(0);
        default:
            //parent
            break;
    }
}

int main(int argc, char** argv)
{
    if(argc != 2)
        exit(EXIT_FAILURE);
    uint16_t port = atoi(argv[1]);
    int sockfd = make_server_socket(port);
    int new_fd,sin_size;
    struct sockaddr_in their_addr;
    sin_size = sizeof (struct sockaddr_in);
    signal(SIGCHLD, sigchld_handler);
    
    while(1)
    {
        if ((new_fd = accept(sockfd, (struct sockaddr *) &their_addr, &sin_size) ) < 0){
            perror("accept() error lol!");
            exit(EXIT_FAILURE);
        }
        else
            printf("accept() is OK...\n");
        
        fork_child_copying_file_to_socket(sockfd,new_fd);
        close(new_fd);
    }
    close(sockfd);
    return 0;
}
