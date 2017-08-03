#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>

void print_char(char letter,int times)
{
    for(int i=0;i<times;i++)
    {
        printf("%c",letter);
        fflush(stdout);
    }
}

void check_error(int pid)
{
    if(pid==-1)
    {
        perror("Fork failed\n");
        exit(1);
    }
}

void print_child_child(int pid2, int times)
{
    if(pid2>0){
        print_char('C',times);
        //wait(0);
    }
    else
        print_char('D',times);
}

void print_parent_child(int pid2, int times)
{
    if(pid2>0){
        print_char('A',times);
        //wait(0);
    }
    else
        print_char('B',times);
}

void fork_process(int *pid1, int *pid2)
{
    *pid1=fork();
    check_error(*pid1);
    *pid2=fork();
    check_error(*pid2);
}

void print_letters(int pid1, int pid2, int times)
{
    if(pid1>0){
        print_parent_child(pid2,times);
        //wait(0);
    }
    else
        print_child_child(pid2,times);
}

void wait_child(int status)//int pid1, int pid2)
{
    printf("\n");
    while(wait(&status)!=-1);
}

int create_processes(int times)
{
    int status, pid1,pid2;
    
    fork_process(&pid1, &pid2);
    
    printf("\npid1: %d\n",pid1);
    printf("\npid2: %d\n",pid2);
    
    print_letters(pid1,pid2,times);
    
    wait_child(status);
    //printf("\n");
    
}

int main(int argc, char *argv[])
{
    int times=10000;
    if(argc!=1)
        times=atoi(argv[1]);
    create_processes(times);
    return 0;
}
