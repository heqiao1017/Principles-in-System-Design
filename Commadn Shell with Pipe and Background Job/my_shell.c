/*
 Command shell. Add pipes and background jobs. Check out this diagram of how pipe might be handled.  I also wrote an example program that implements the command cat < in | cat > out in this file. Only the first command in a pipeline can have its input redirected to come from a file and only the last command in a pipeline can have its output redirected to a file, so you can execute these commands:
 % tee newOutputFile1 > newOutputFile2 < existingInputFile &
 % cat < existingInputFile | tee newOutputFile1 > newOutputFile2
 % cat < existingInputFile | tee newOutputFile1 > newOutputFile2 &
 % cat < existingInputFile | tr A-Z a-z | tee newOutputFile1 | tr a-z A-Z > newOutputFile2
 % cat < existingInputFile | tr A-Z a-z | tee newOutputFile1 | tr a-z A-Z > newOutputFile2 &

 */



#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>

#define MAX 256
typedef char buffer[MAX];
typedef char * vector[MAX];
buffer arg_buffer[MAX];

enum Mode
{
    PLAIN,
    BACKGROUND,
    PIPE
};

struct command
{
    buffer name;
    vector argv;
    char * ifile;
    char * ofile;
    enum Mode mode;
    int is_last;
    char * full_path;
};

char * path_values[MAX];

struct process
{
    buffer cmd;
    int pid;
    enum Mode mode;
};


struct process process_stack[MAX];
int psp = -1;

int in_pipe = 0;
//int pass_through;

void token_path(char **pv, char *path)
{
    char *token=strtok(path,":");
    while(token!=NULL)
    {
        *pv++ = token;
        token=strtok(NULL,":");
    }
    *pv = NULL;
}

void get_path()
{
    char * path, * * pv;
    path = getenv("PATH");
    pv = path_values;
    token_path(pv,path);
}

const char delim[]="<>&|;";

char get_first_char(char ch)
{
    for (;;)
    {
        if ((ch = getchar()) == EOF) exit(0);
        if (ch != ' ' && ch != '\t') break;
    }
    return ch=='\n'? 0:ch;
}


void get_rest_char(char **p, char ch)
{
    if (strchr(delim, ch) == NULL)
    {
        for (;;)
        {
            if ((ch = getchar()) == EOF) exit(0);
            if (isspace(ch) || strchr(delim, ch)) break;
            *(*p)++ = ch;
        }
        ungetc(ch, stdin);
    }
    **p = 0;
}

char * get_word(char * word)
{
    int ch;
    char * p = word;
    
    if(!(ch=get_first_char(ch))) return NULL;
    
    *p++ = ch;
    get_rest_char(&p,ch);
    return word;
}

void initialize_cmd(struct command * cmd)
{
    cmd->ifile = NULL;
    cmd->ofile = NULL;
    cmd->mode = PLAIN;
    cmd->is_last = 0;
}

int get_command_name(struct command * cmd)
{
    if (!get_word(cmd->name))
    {
        cmd->name[0] = 0;
        cmd->is_last = 1;
        return 0;
    }
    return 1;
}

/* EXCEEDING 5 LINES BY 2 */
int get_rest_argments(struct command * cmd)
{
    char * * argv = cmd->argv;
    buffer * arg_buf_ptr = arg_buffer;
    for (*argv++=cmd->name; *argv = get_word(*arg_buf_ptr); argv++, arg_buf_ptr++){
        switch(*argv[0]){
            case '<':
                if (!(cmd->ifile = get_word(*arg_buf_ptr)))
                    fprintf(stderr, "Redirect w/o file\n");
                argv--;
                break;
            case '>':
                if (!(cmd->ofile = get_word(*arg_buf_ptr)))
                    fprintf(stderr, "Redirect w/o file\n");
                argv--;
                break;
            case '&':
                cmd->mode = BACKGROUND;
                *argv = NULL;
                return 0;
            case '|':
                cmd->mode=PIPE;
                *argv = NULL;
                return 0;
            case ';':
                *argv = NULL;
                return 0;
            default:
                break;
        }
    }
    return 1;
}

void get_command_line(struct command * cmd)
{
    initialize_cmd(cmd);
    if(!get_command_name(cmd)) return;
    if(!get_rest_argments(cmd)) return;
    cmd->is_last = 1;
}

int wait_children(int status,int *pid, int *i)
{
    *pid = wait(&status);
    if ((*pid) <= 0) return -1;
    for ((*i) = 0; (*i) <= psp; (*i)++)
        if (process_stack[*i].pid == *pid) break;
    return 1;
}

void do_process_stack(int pid, int i)
{
    if (i <= psp)
    {
        if (process_stack[i].mode == BACKGROUND)
            fprintf(stderr, "[%s] Done\n", process_stack[i].cmd);
        process_stack[i] = process_stack[psp--];
    }
}

int check_children()
{
    int status, pid, i, j;
    if(wait_children(status,&pid,&i)==-1)
        return -1;
    do_process_stack(pid,i);
    return pid;
}


int readable(char * file_name)
{
    struct stat buf;
    if (stat(file_name, &buf) < 0) return 0;
    return buf.st_mode & S_IRUSR;
}

int writable(char * file_name)
{
    struct stat buf;
    if (stat(file_name, &buf) < 0) return 1;
    return buf.st_mode & S_IWUSR;
}


char * expand_path(char * name)
{
    char * * pv;
    static buffer path_part;
    int fd;
    
    for (pv = path_values; *pv; pv++)
    {
        sprintf(path_part, "%s/%s", *pv, name);
        if (readable(path_part)) return path_part;
    }
    return NULL;
}


int can_redirect_in_pipe(struct command * cmd)
{
    if ((cmd->mode == PIPE && in_pipe
         &&(cmd->ifile || cmd->ofile))
        || cmd->mode == PIPE && !in_pipe && cmd->ofile
        || cmd->mode != PIPE && in_pipe && cmd->ifile)
    {
        fprintf(stderr, "Can't redirect in pipeline\n");
        return 0;
    }
    return 1;
}

int can_open_input_file(struct command * cmd)
{
    if (cmd->ifile && !readable(cmd->ifile))
    {
        fprintf(stderr, "Unable to open input file '%s'\n", cmd->ifile);
        return 0;
    }
    return 1;
}
int can_open_output_file(struct command * cmd)
{
    if (cmd->ofile && !writable(cmd->ofile))
    {
        fprintf(stderr, "Unable to open output file '%s'\n", cmd->ofile);
        return 0;
    }
    return 1;
}

int can_find_command(struct command * cmd)
{
    if (!(cmd->full_path = expand_path(cmd->name)))
    {
        fprintf(stderr,  "Unable to find command '%s'\n", cmd->name);
        return 0;
    }
    return 1;
}

int verify_command_line(struct command * cmd)
{
    if (!can_redirect_in_pipe(cmd)) return 0;
    if(!can_open_input_file(cmd)) return 0;
    if(!can_open_output_file(cmd)) return 0;
    if(!can_find_command(cmd)) return 0;
    return 1;
}

void open_file(struct command * cmd,int * in, int * out)
{
    *in = 0, *out = 1;
    if (cmd->ifile)
        *in = open(cmd->ifile, O_RDONLY);
    if (cmd->ofile)
        *out = open(cmd->ofile, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
}

int create_pipe(int pipes[])
{
    if ( pipe(pipes) == -1)
    {
        fprintf(stderr, "Unable to create pipes\n");
        return 0;
    }
    return 1;
}

int process_pipe(int pipes[], int * in, int * out,int* pass_through)
{
    if(!create_pipe(pipes)) return 0;
    if (in_pipe) *in = *pass_through;
    else in_pipe = 1;
    *out = pipes[1];
    *pass_through = pipes[0];
}

void finish_read_pipe(int * in, int pass_through)
{
    in_pipe = 0;
    *in = pass_through;
}

int check_pipe(struct command * cmd,int pipes[], int * in, int * out,int* pass_through)
{
    if (cmd->mode == PIPE)
        return process_pipe(pipes,in,out,pass_through);
    else if (in_pipe)
        finish_read_pipe(in,*pass_through);
    return 1;
}

int set_child_io(struct command * cmd, int * in, int * out)
{
    static int pass_through;
    int pipes[2];
    open_file(cmd,in,out);
    return check_pipe(cmd,pipes,in,out, &pass_through);
}

void do_child_work(int in, int out, struct command cmd,char *envp[])
{
    dup2(in,0);
    dup2(out,1);
    execve(cmd.full_path,cmd.argv, envp);
    fprintf(stderr, "Unable to run '%s'\n", cmd.full_path);
    exit(-1);
}

void close_file_descripter(int in, int out)
{
    if (in != 0)
        close(in);
    if (out != 1)
        close(out);
}

void process_unplain_mode(struct command cmd,int child_pid)
{
    process_stack[++psp].pid = child_pid;
    strcpy(process_stack[psp].cmd, cmd.name);
    process_stack[psp].mode = cmd.mode;
    if (cmd.mode == BACKGROUND)
        fprintf(stderr, "[%d] %d\n", psp, child_pid);
}

void do_parent_work(struct command cmd,int child_pid,int in, int out)
{
    if (cmd.mode == PLAIN)
        while (check_children() != child_pid);
    else
        process_unplain_mode(cmd,child_pid);
    close_file_descripter(in, out);
}

void create_process(int *child_pid, int in, int out,struct command cmd,char *envp[])
{
    switch(*child_pid = fork())
    {
        case -1:
            perror("Fork failed\n");
            exit(-1);
        case 0:
            do_child_work(in,out,cmd,envp);
        default:
            do_parent_work(cmd,*child_pid,in,out);
            break;
    }
}


void do_rest_work_then_fresh_start(struct command cmd)
{
    if (psp >= 0)
        while (check_children() >= 0);
    
    if (cmd.is_last)
    {
        printf("%% ");
        fflush(stdout);
    }
}

/* EXCEEDING 5 LINES BY 2 */
void do_my_shell(struct command cmd,int child_pid,int in, int out, char *envp[])
{
    while (1)
    {
        do_rest_work_then_fresh_start(cmd);
        get_command_line(&cmd);
        if (!cmd.name[0]) continue;
        if (!verify_command_line(&cmd)) continue;
        if (!set_child_io(&cmd, &in, &out)) continue;
        create_process(&child_pid,in,out,cmd,envp);
    }
    
}

int main(int argc, char *argv[], char *envp[])
{
    struct command cmd;
    int child_pid, in, out;
    
    get_path();
    cmd.is_last = 1;
    do_my_shell(cmd, child_pid,in,out,envp);
    return 0;
}
