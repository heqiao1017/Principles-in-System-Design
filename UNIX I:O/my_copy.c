/*
 UNIX I/O Exercise: The purposes of this assignment are (1) to become familiar with UNIX I/O system calls and (2) to see the difference in performance between C library functions and system calls.  The program you write should do the following.
my_copy is a main program that prints out the time to copy the file using each of three methods.  The name of the input file and the name of the output file should come from the command line arguments to your program (via argv).
Method 1: Implement a function that uses fopen(), fgetc() and fputc(), then fclose() to copy the input file to the output file one character at a time. Each loop iteration will read and write a single character but I/O will be buffered by the stdio library routines which make system calls read() and write() to fill or flush the stdio buffers.
Method 2: Implement a function that uses the system calls open(), read() and write(), then close() to copy the input file to the output file one character at a time. Each loop iteration will read and write just a single character.  There will be no buffering because you will be making system calls to read and write each character.
Method 3: Implement a function that uses the system calls open(), read() and write(), then close() to copy the input file to the output file one buffer at a time (the buffer should be of size "BUFSIZ", which is declared in the <stdio.h> include file and each read or write should write as many characters as possible per loop iteration up to BUFSIZ).
Once these functions are implemented, test them out by having them perform their work N times each (where N is one of the command-line arguments - see last bullet below for details of arguments).  Display the amount of wallclock, user, and system time it takes for each of the three method functions to execute.
Use a Timer to measure the performance of each of the three approaches to copying a file.  Use /usr/share/dict/linux.words for the input file to copy.  Here is a Timer you may use. There is an example of use at the bottom https://docs.google.com/document/d/1KY2DuvijtOKs7B4zlLK18lhBLdg1UDUtim8k5Bphudc/edit?usp=sharing  You must split this file into Timer.h and Timer.c as described in the comment at the top, then compile Timer.c with -c option to create Timer.o which you can link in with your program that uses the timer. Your program that uses the Timer must #include “Timer.h”
Your program will take four arguments: argv[1] will be a number 1, 2, or 3 indicating which of the above methods your program should use to make the copy; argv[2] will be the input file name; argv[3] will be the output file name, and argv[4] will be the number of times to repeat the file copy (it should default to 1 if only three arguments are passed in). A given invocation of your program should make N copies using the method passed in for argv[1].  Ideally, the output should be just the time required to make the copy, but we are making N copies so it will also include the time to open and close the file for each copy.  To ensure you measure the entire N file copies, start the timer (with Timer_init()) right before you loop to make N copies and stop the timer just after you have made the Nth copy.  Print the wallclock, user, and system time (to stdout) after you have stopped the timer.  Examples of each call (using xyz for times)
		$ my_copy 1 /usr/share/dict/linux.words output1 1
wallclock x, user y, system time z (predict a moderate speed)
		$ my_copy 2 /usr/share/dict/linux.words output2 1
wallclock x, user y, system time z (predict a *really* slow speed)
		$ my_copy 3 /usr/share/dict/linux.words output3 1
wallclock x, user y, system time z (predict a really fast speed)
*/


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

