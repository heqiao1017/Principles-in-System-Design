#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "Timer.h"

void open_input_file_method2and3(char *in,int *open_in)
{
    if((*open_in=open(in,O_RDONLY))<0){
        fprintf(stderr, "Cannot open %s\n",in);
        exit(1);
    }
    
}
void open_output_file_method2and3(char *out,int *open_out,mode_t mode)
{
    if((*open_out = open(out, O_WRONLY | O_CREAT | O_TRUNC, mode)) <0){
        fprintf(stderr, "Cannot open %s\n",out);
        exit(1);
    }
}

void close_file_and_print_time_methos2and3(int open_in,int open_out ,double wallclock,double user_time,double system_time)
{
    close (open_in);
    close (open_out);
    printf("wallclock %lf, user %lf, system time %lf\n", wallclock,user_time,system_time);
}

/* EXCEEDING 5 LINES BY 1 */
void read_copy_file_method3(int open_in, int open_out, int read_in,double *wallclock,double *user_time,double *system_time)
{
    char buff[BUFSIZ];
    Timer_start();
    while((read_in=read(open_in,&buff,BUFSIZ))>0)
        if(write(open_out,&buff,read_in)<0)
            perror("write in method3");
    
    Timer_elapsedTime(wallclock,user_time,system_time);
}

void method3(char *in, char *out)
{
    double wallclock,user_time,system_time;
    int open_in,open_out,read_in;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    
    open_input_file_method2and3(in,&open_in);
    open_output_file_method2and3(out,&open_out,mode);
    read_copy_file_method3(open_in,open_out,read_in,&wallclock,&user_time,&system_time);
    close_file_and_print_time_methos2and3(open_in,open_out,wallclock,user_time,system_time);

}

void read_copy_file_method2(char c,int open_in, int open_out,double *wallclock,double *user_time,double *system_time)
{
    Timer_start();
    while(read(open_in,&c,1)>0)
        if(write(open_out,&c,1)<0)
            perror("write in method2");
    
    Timer_elapsedTime(wallclock,user_time,system_time);
}

void method2(char *in, char *out)
{
    double wallclock,user_time,system_time;
    char c;
    int open_in,open_out;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    
    
    open_input_file_method2and3(in,&open_in);
    open_output_file_method2and3(out,&open_out,mode);
    read_copy_file_method2(c,open_in,open_out,&wallclock,&user_time,&system_time);
    close_file_and_print_time_methos2and3(open_in,open_out,wallclock,user_time,system_time);
    
}

FILE *open_input_file_method1(FILE* ip,char *in)
{
    ip=fopen(in,"r");
    if ( ip == NULL ) {
        fprintf(stderr, "Can't open %s\n", in);
        exit(1);
    }
    return ip;
}
FILE *open_output_file_method1(FILE* op, char *out)
{
    op=fopen(out,"w");
    if ( op == NULL ) {
        fprintf(stderr, "Can't open %s!\n", out);
        fclose(op);
        exit(1);
    }
}

void read_copy_file_method1(FILE* ip,FILE* op, double *wallclock,double *user_time,double *system_time)
{
    int c;
    Timer_start();
    while((c=fgetc(ip))!=EOF)
        fputc(c,op);
    Timer_elapsedTime(wallclock,user_time,system_time);
}

void close_file_and_print_time(FILE* ip,FILE* op,double wallclock,double user_time,double system_time)
{
    fclose(ip);
    fclose(op);
    printf("wallclock %lf, user %lf, system time %lf\n", wallclock,user_time,system_time);
}

void method1(char *in, char *out)
{
    FILE *ip, *op;
    double wallclock,user_time,system_time;
    
    ip=open_input_file_method1(ip,in);
    op=open_output_file_method1(op,out);
    read_copy_file_method1(ip,op,&wallclock,&user_time,&system_time);
    close_file_and_print_time(ip,op,wallclock,user_time,system_time);
    
}

void copy_file(int repeat,int method_num,char *input_file, char *output_file)
{
    for(int i=0;i<repeat;i++){
        switch(method_num){
            case 1:
                method1(input_file, output_file);
                break;
            case 2:
                method2(input_file, output_file);
                break;
            case 3:
                method3(input_file, output_file);
                break;
            default:
                fprintf(stderr, "Invalid method number\n");
                exit(1);
        }
    }
}

void assign_command(int argc, char *argv[],char **input_file, char **output_file, int *repeat)
{
    *input_file=argv[2];
    *output_file=argv[3];
    if(argc==5)
        *repeat=atoi(argv[4]);
}

int main(int argc, char *argv[])
{
    char *input_file, *output_file;
    int repeat=1, method_num=atoi(argv[1]);
    
    assign_command(argc,argv,&input_file,&output_file,&repeat);
    copy_file(repeat,method_num,input_file,output_file);
    
    return 0;
}

