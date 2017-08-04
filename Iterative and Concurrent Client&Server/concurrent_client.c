#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <sys/wait.h>

#define h_addr h_addr_list[0]
struct sockaddr_in their_addr;
int make_client_socket (uint16_t port, char* hostname)
{
    int sockfd;
    struct hostent *he;
    
    if((he=gethostbyname(hostname)) == NULL)
    {
        perror("gethostbyname()");
        exit(1);
    }
    else
        printf("Client-The remote host is: %s\n", hostname);
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1)
    {
        perror("socket() error lol!");
        exit(EXIT_FAILURE);
    }
    else
        printf("Client-socket() is OK...\n");
    
    their_addr.sin_family = AF_INET;
    their_addr.sin_port = htons(port);
    their_addr.sin_addr = *((struct in_addr *)he->h_addr);
    memset(&(their_addr.sin_zero), '\0', 8);
    
    return sockfd;
}

char * base_name(char *path)
{
    return strrchr(path,'/');
}

void sigchld_handler(int sig)
{
    while (waitpid(-1, 0, WNOHANG) > 0);
    return;
}

void copy_file(int sockfd, char* filename)
{
    int read_in, file_open;
    char buff[BUFSIZ];
    memset(buff, 0, BUFSIZ);
    
    if((file_open= open(filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) <0){
        fprintf(stderr, "Cannot open to write\n");
        exit(1);
    }
    while((read_in=read(sockfd,buff,BUFSIZ))>0)
    {
        if(write(file_open,buff,read_in)<0){
            perror("copy to file");
            exit(EXIT_FAILURE);
        }
        //memset(buff, 0, BUFSIZ);
    }
    close(file_open);
}

void fork_child_copying_file_from_socket(int sockfd, char * full_path, char* filename)
{
    int pid;
    switch(pid=fork()){
        case -1:
            perror("Fork failed\n");
            exit(EXIT_FAILURE);
        case 0:
            if(write(sockfd,full_path,strlen(full_path)+1)<0){
                perror("Write to server");
                exit(EXIT_FAILURE);
            }
            copy_file(sockfd,filename);
            close(sockfd);
            exit(0);
        default:
            break;
    }
    
}

int main(int argc, char** argv)
{
    if(argc < 4)
        exit(EXIT_FAILURE);
    uint16_t port = atoi(argv[2]);
    signal(SIGCHLD, sigchld_handler);
    for(int i=3;i<argc;i++)
    {
        int sockfd = make_client_socket(port, argv[1]);
        if(connect(sockfd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1)
        {
            perror("connect()");
            exit(EXIT_FAILURE);
        }
        else
            printf("Client-The connect() is OK...\n");
        
        char *filename=base_name(argv[i]);
        fork_child_copying_file_from_socket(sockfd,argv[i],++filename);
        close(sockfd);
    }
    return 0;
}
