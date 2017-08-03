#include "my_ls.h"


void print_time(char * time_buffer, int n)
{
    char * token=strtok(time_buffer," :");
    while(token && n>0){
        printf(" %s",token=strtok(NULL," :"));
        n--;
    }
    printf(":%s ",strtok(NULL," :"));
}

void print_modification_date(struct stat s)
{
    struct tm lt;
    char time_buffer[80];
    time_t t = s.st_mtime;
    
    localtime_r(&t, &lt);
    strftime(time_buffer, sizeof(time_buffer), "%c", &lt);
    print_time(time_buffer,3);
}


char * get_group_name(gid_t group_id)
{
    struct group *gr = getgrgid(group_id);
    if(gr)
        return gr->gr_name;
    return "";
}

char * get_owner_name(uid_t user_id)
{
    struct passwd *pw=getpwuid(user_id);
    if(pw)
        return pw->pw_name;
    return "";
}


void print_mode(mode_t mode)
{
    printf("%s%s%s%s",(S_ISDIR(mode))? "d" : "-",
           (mode & S_IRUSR)==0? "-" : "r",
           (mode & S_IWUSR)==0? "-" : "w",
           (mode & S_IXUSR)==0? "-" : "x");
    
    printf("%s%s%s",(mode & S_IRGRP)==0? "-" : "r",
           (mode & S_IWGRP)==0? "-" : "w",
           (mode & S_IXGRP)==0? "-" : "x");
    
    printf("%s%s%s ",(mode & S_IROTH)==0? "-" : "r",
           (mode & S_IWOTH)==0? "-" : "w",
           (mode & S_IXOTH)==0? "-" : "x");
}

char* readable_size(double size,char * result)
{
    char * unit[]={"","K","M","G"};
    int i=0;
    
    for(;size>1024;i++)
        size/=1024;
    
    sprintf(result,"%.*f%s",i,size,unit[i]);
    return result;
}


void print_size_modification_time_filename(struct stat s, char* filename, char *buffer)
{
    printf("%s",readable_size(s.st_size,buffer));
    print_modification_date(s);
    printf("%s\n",filename);
}

void print_mode_user_group(struct stat s)
{
    print_mode(s.st_mode);
    printf("%d ",(int)s.st_nlink);
    printf("%s ",get_owner_name(s.st_uid));
    printf("%s ",get_group_name(s.st_gid));
}

void show_info(struct stat s,char* filename,char *buffer)
{
    print_mode_user_group(s);
    print_size_modification_time_filename(s,filename,buffer);
}

void print_file_info(char *path_buffer,char* filename)
{
    struct stat s;
    char buffer[80];
    
    if((stat(path_buffer,&s))==-1)
        fprintf(stderr, "Error(%d) stat %s\n", errno, filename);
    else
        show_info(s,filename,buffer);
}

void do_ls(char* dir)
{
    DIR *dirp;
    struct dirent *entry;
    if((dirp  = opendir(dir))==NULL){
        fprintf(stderr, "Error(%d) opening %s\n", errno, dir);
        exit(-1);
    }
    
    char path[1024];//,path_buffer[1024];
    
    while((entry=readdir(dirp)))
    {
        if( strcmp(".",entry->d_name)!=0 && strcmp("..",entry->d_name)!=0){
            sprintf(path,"%s/%s",dir,entry->d_name);
            print_file_info(path,entry->d_name);
        }
    }
    
    closedir(dirp);
}
