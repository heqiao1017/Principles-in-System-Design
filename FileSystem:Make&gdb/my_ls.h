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
