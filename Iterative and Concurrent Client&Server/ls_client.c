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

int read_from_server(int fd,char *buffer)
{
    int nbytes;
    nbytes = read (fd, buffer, BUFSIZ);

    
    if (nbytes < 0)
    {
        perror ("read");
        exit (EXIT_FAILURE);
    }
    else if (nbytes == 0)
    {
        close(fd);
        return -1;
    }
    else
    {
        return 1;
    }

}

int main(int argc, char** argv)
{
    if(argc < 3)
        exit(EXIT_FAILURE);
    uint16_t port = atoi(argv[2]);
    
    
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
        
        printf("arg: %s\n",argv[i]);
        
        write(sockfd,argv[i],strlen(argv[i])+1);
        
        char buffer[BUFSIZ];
        memset(buffer, 0, BUFSIZ);

        while(read_from_server(sockfd,buffer)>0)
        {
            printf("%s",buffer);
            fflush(stdout);
            memset(buffer, 0, BUFSIZ);

        }

        close(sockfd);
    }
    
    return 0;
}
