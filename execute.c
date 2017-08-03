/*
Write a C program, called execute, that reads a one line Bash command from the standard input and executes the program, 
given in the first word of a line, passing in the arguments, given as the rest of the words on the command line.  
You may use strtok() to parse the command into words, but you must build an array of c-strings for argv.  
argv[0] must be the full path to the command, argv[1] will be the first argument and so on.  
The program should search down the PATH variable to find the full path of the executable before running it.  
strtok(), sprintf(), and stat() should be helpuf in constructing the full path to the command. 
PATH will be in the list of environment variables in the third argument to main, envp.  
You should call execve() with appropriate full path and argv you construct to run the command. Here are some sample runs:
	echo /bin/echo hello world | execute
echo which ls echo login | execute
execute < my_command
where my_command contains
ls -lR /usr
*/


#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>

/* EXCEEDING 5 LINES BY 3 */
void parse(char* buff, char* new_args[])
{
    while(*buff != '\0')
    {
        while(*buff == ' ' || *buff == '\n' || *buff == '\t')
        {
            *buff = '\0';
            buff++;
        }
        *new_args++ = buff;
        while(*buff != '\0' && *buff != ' ' && *buff != '\t' && *buff != '\n')
        {
            buff++;
        }
    }
    *new_args = 0;
}

void execute_on_valid_path(struct stat s,char *const envp[], char *filename,char *new_argvs[])
{
    if(stat(filename,&s)==0){
        new_argvs[0]=filename;
        execve(filename,new_argvs,envp);
    }
}

void execute_full_path(char *path,char *const envp[], char *filename,char *new_argvs[],struct stat s)
{
    char *token=strtok(path,":");
    while(token!=NULL)
    {
        sprintf(filename,"%s/%s",token, new_argvs[0]);
        execute_on_valid_path(s,envp,filename,new_argvs);
        token=strtok(NULL,":");
    }
}

void process_command(char *const envp[], char *filename,char *new_argvs[])
{
    struct stat s;
    char path[1024];

    strcpy(path,getenv("PATH"));
    
    if(execve(new_argvs[0],new_argvs,envp)<0)
        execute_full_path(path,envp,filename,new_argvs,s);

}

int main(int argc, char *argv[],char *const envp[])
{
    char filename[1024];
    char buff[1024];
    char *new_argvs[1024];


    fgets(buff,1024,stdin);
    parse(buff,new_argvs);
    process_command(envp,filename,new_argvs);
    
    return 0;
}
