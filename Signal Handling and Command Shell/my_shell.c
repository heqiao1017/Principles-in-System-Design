/*
 Command shell. Write a program called my_shell, which is a step towards a simple command shell to process commands as follows:  print a prompt (e.g., “% “) then read a command on one line, then evaluate it.  A command can have the following form:
 commandName [ argumentList ] [ IORedirect ] [ IORedirect ]
 Where an argumentList is a list of zero or more words (no patterns allowed), IORedirect is either “< inFile” or “> outFile” or both where inFile and outFile are valid Unix file names. You will use versions of the system calls fork(), open(), dup2(), exec(), and wait() for this assignment.
 You should test my_shell with these commands: (note # to end of line is a comment to you to help you understand what the command is supposed to do. existingInputFile means you have a file in your current directory that contains some text. newOutputFile means it will be created by the execution of that command.)
 % /usr/bin/tee newOutputFile  # input from and output to the terminal
 % /usr/bin/tee newOutputFile < existingInputFile
 % /usr/bin/tee newOutputFile1 < existingInputFile > newOutputFile2
*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

char *new_args[50];
int in_direct=0, out_direct=0, in_direct_index=0, out_direct_index=0;

void reset()
{
    in_direct=0;
    out_direct=0;
    in_direct_index=0;
    out_direct_index=0;
}

void parse_args(char *buff, char **args)
{
    while(*buff)
    {
        switch(*buff)
        {
            case ' ':
            case '\t':
            case '\n':
                *buff++=0;
                break;
            case '<':
                *args++="<";
                *buff++=0;
                in_direct=1;
                break;
            case '>':
                *args++=">";
                *buff++=0;
                out_direct=1;
                break;
            default:
                *args++=buff;
                while(*buff && *buff != ' ' && *buff != '\t' && *buff != '\n' && *buff != '<' && *buff != '>')
                    buff++;
                break;
        }
    }
    *args=0;
}

void redirect_input(int readin)
{
    dup2(readin,0);
    close(readin);
}

void redirect_output(int writeout)
{
    dup2(writeout,1);
    close(writeout);
}

void check_redirect(int readin,int writeout)
{
    if(in_direct!=0)
        redirect_input(readin);
    if(out_direct!=0)
        redirect_output(writeout);
}

void create_args_without_ioredirection(char **arg_lst)
{
    int limit=(in_direct_index!=0? in_direct_index:out_direct_index);
    int i=1;
    for(;i<limit;i++)
        new_args[i]=arg_lst[i];
    new_args[i]=NULL;
}

void execute_with_iodirection(char **arg_lst)
{
    new_args[0]=arg_lst[0];
    create_args_without_ioredirection(arg_lst);
    execve(arg_lst[0],new_args,NULL);
}

void execute(char **arg_lst,int readin,int writeout)
{
    check_redirect(readin,writeout);
    if(in_direct==0 && out_direct==0)
        execve(arg_lst[0],arg_lst,NULL);
    else
        execute_with_iodirection(arg_lst);
}

void close_file(int readin,int writeout)
{
    if(readin!=-1)
        close(readin);
    if(writeout!=-1)
        close(writeout);
}

void process(char **arg_lst,int readin,int writeout)
{
    int pid, status;
    switch(pid = fork())
    {
        case -1:
            perror("Fork failed\n");
            exit(1);
        case 0:
            execute(arg_lst,readin,writeout);
            break;
        default:
            while(waitpid(-1,&status,WNOHANG)>=0)
                continue;
            close_file(readin,writeout);
            break;
    }
    reset();
}

/* EXCEEDING 5 LINES BY 4 */
void open_file(char **arg_lst, int *readin, int *writeout)
{
    for(int i=0;arg_lst[i];i++)
    {
        if(strcmp(arg_lst[i],"<")==0){
            in_direct_index=i;
            if((*readin = open(arg_lst[i+1], O_RDONLY))<0)
                perror(arg_lst[i+1]);
            
        }
        else if(strcmp(arg_lst[i],">")==0){
            out_direct_index=i;
            if((*writeout = open(arg_lst[i+1], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH))<0)
                perror(arg_lst[i+1]);
        }
    }
}

/* EXCEEDING 5 LINES BY 2 */
void do_my_shell(char *buff, char **arg_lst)
{
    while(1)
    {
        int readin=-1,writeout=-1;
        printf("%% ");
        fgets(buff,1024,stdin);
        parse_args(buff,arg_lst);
        open_file(arg_lst,&readin,&writeout);
        process(arg_lst, readin,writeout);
    }
}

int main()
{
    char buff[1024];
    char *arg_lst[100];
    do_my_shell(buff,arg_lst);
    return 0;
}


