/*

ICS 53
HW 3
(30 points) make and gdb
Create a subdirectory under your hw directory called hw3 and put all files related to this homework in that directory.
Define a Makefile in hw3 to maintain the program you are writing for this homework assignment (my_ls). The default target should make my_ls executable and include gdb debugging information in the executable.  Define an additional target called `clean` to remove any files created by a make of your program (e.g., each specific .o file and all the executables - do not use wildcards!). Write your Makefile by hand using vi or vim (same thing). Do not use any of make’s default rules. To test your Makefile, use ‘touch’ to modify various files (like each of the .h files or the .c files), then give a make command to show that only the dependent files are recompiled.  The default make should remake your program my_ls correctly. Divide your program into the following three files: my_ls.h (declares do_ls(char *file)), my_ls.c (includes my_ls.h and defined do_ls()), main.c (includes my_ls.h and calls do_ls on appropriate program arguments).
Use the debugger, gdb, while developing your program to help you become more familiar with gdb commands and to help you track down errors.  Use the main debugger commands presented in lecture this week. Set breakpoints, set watches (if you can figure them out), run your program with command line arguments within the debugger, single stepping through your program (probably mixing up next and step), printing out the call stack from a breakpoint or from when the program is stopped due to segmentation violation.  You may skip trace unless you can figure out how to make it work.
What to submit: connect to your hw3 directory, start a script called hw3.script (by running the Unix command script which records your interactive session with bash - as described in lecture), make clean, make your program, run gdb on your program, set breakpoint (and a watch if you can), run your program with arguments (like a directory to list), when you stop in a break point, print out some variables or parameters, print out the back trace, single step using both step and next, then continue execution until your program finishes, quit gdb, then make clean, cat your Makefile, then exit your script. Submit a zip file of the entire contents of your hw3 directory which includes Makefile, my_ls.c, hw3.script, but not any .o files or my_ls executable.
(70 points) Write a C program, called my_ls, that behaves like the Unix command ls -lR, e.g.,
$ ls -lR filelist
Use `readdir` and `stat` to write a C program, called my_ls, to implement a subset of long, recursive `ls`. Files must be listed one name per line but additionally indented by the nesting depth of the directory. If there are no arguments to my_ls, list the files in "." (dot) the current working directory. Otherwise, for each command-line argument N, do the following. If N is a directory, print the detailed info for the directory (same as you would for a plain file as below for myFile), then indent 4 spaces and do recursive my_ls of each entry in N (i.e., like ls -lR, but you do not need to sort the files). If N is a file, print the following information for file N in the format (like ls -l):
                    mode  num_links  owner  group  size_in_bytes   modification_timedate   name
Specific Example:
$ my_ls myFile
-rw-rw-r-- 1 klefstad faculty 100 Feb 26 07:08 myFile
Explanation of example (see http://en.wikipedia.org/wiki/Chmod for mode details)
-rw-rw-r- mode (file type and access permissions)
1 : number of hard-links to this file
klefstad: owner of the file
faculty: group of the file
100: size of the file in bytes
Feb 26 07:08 last modification date and time
myFile: file/directory name
You need not handle symbolic links or devices (just files and directories).  Here are some snippets of code for inspiration (but don’t copy and paste).  One reads the files in a specified directory.  The other converts a user ID integer into a symbolic string.  Note you must not use “scandir” which is a C-lib function that reads a directory, but will leak memory unless you free each file name.  Print the mode using the same symbolic format used by ls.  Print the user name as a symbolic string, like klefstad, not as an integer.

*/



#ifndef MY_LS_H
#define MY_LS_H
#include <stdio.h>
#include <math.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

void print_time(char * time_buffer, int n);
void print_modification_date(struct stat s);
char * get_group_name(gid_t group_id);
char * get_owner_name(uid_t user_id);
void print_mode(mode_t mode);
char* readable_size(double size,char * result);
void print_size_modification_time_filename(struct stat s, char* filename, char *buffer);
void print_mode_user_group(struct stat s);
void show_info(struct stat s,char* filename,char *buffer);
void print_file_info(char *path_buffer,char* filename);



void do_ls(char* dir);

#endif
