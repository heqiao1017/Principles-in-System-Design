/*
Write 10 of the library functions defined by string.h for working with c-strings as documented here 
http://www.tutorialspoint.com/c_standard_library/string_h.htm. http://www.codingunit.com/c-tutorial-strings-and-string-library-functions
You are to write strlen, strcpy, strcat, strcmp, strncmp, strchr, strpbrk, strstr, strspn, strtok.  
Write a C program to test your implementations of each of your string/memory functions from string.h.  
Write at least three test cases per string function.  main can call each of the 10 test function (so it may be longer than 5 lines).  
Write one test function for each string function that has at least three test cases.  These test function may also exceed the 5 line limit.  I started the main function for you here so use this as a start and follow my example for how to grow this for all 10 functions.
*/


#include <stdio.h>
#include <stdlib.h>

#define require(e) if (!(e)) fprintf(stderr, "FAILED line %d        %s: %s       %s\n", __LINE__, __FILE__, __func__, #e)


unsigned long strlen(const char * str)
{
    return ! *str ? 0 : 1+strlen(str+1);
}

char *strcpy(char *dest, const char *src)
{
    int i=0;
    for(; src[i] ; ++i)
        dest[i] = src[i];
    dest[i] = '\0';
    return dest;
}


char *strcat(char *dest, const char *src)
{
    int dest_len = strlen(dest);
    
    strcpy(dest+dest_len, src);
    
    return dest;
}


int strcmp(const char *str1, const char *str2)
{
    /*while(*str1 && *str2)
     if((*str1++)!=(*str2++))
     return (*--str1)-(*--str2);
     
     return (*str1)-(*str2);*/
    return !*str1 || !*str2 || *str1!=*str2 ? *str1-*str2: strcmp(str1+1,str2+1);
}


int my_strncmp(const char *str1, const char *str2, int n)
{
    for(int i=0;i<n;++i)
        if(str1[i]!=0 && str2[i]!=0 && str1[i]!=str2[i])
            return str1[i]-str2[i];
    
    return 0;
}

char *my_strchr(const char *str, int c)
{
    while(*str)
        if(*str++ == (char)c)
            return (char *)--str;
    return 0;
}


char *strpbrk(const char *str1, const char *str2)
{
    while(*str1)
        if(my_strchr(str2, *str1++))
            return (char *)--str1;
    return 0;
}

char *strstr(const char *haystack, const char *needle)
{
    int len=strlen(needle);
    while(*haystack)
        if(my_strncmp(haystack++,needle,len)==0)
            return (char *)--haystack;
    return 0;
}

unsigned long strspn(const char *str1, const char *str2)
{
    int len=0;
    while(*str1 && my_strchr(str2,*str1++))
        len++;
    return len;
}

//***************************************************

//strtok helper func
void copy_n_char(char *dest, const char *src, int n, int * i)
{
    while((*i)<n)
    {
        if(src[*i])
        {
            dest[*i]=src[*i];
            (*i)++;
        }
    }
}

//strtok helper func
char *mystrncpy(char *dest, const char *src, int n)
{
    int i=0;
    copy_n_char(dest,src,n,&i);
    dest[i]='\0';
    return dest;
}

//strtok helper func
char * return_the_last_token(char **token_from)
{
    char* to_return=*token_from;
    *token_from=NULL;
    return to_return;
}

//strtok helper func
char* abstract_token_from_source(char **token_from,char * token_left)
{
    int differ = strlen(*token_from)-strlen(token_left);
    char *to_return= malloc(differ+1);
    to_return=mystrncpy(to_return,*token_from,differ);
    *token_from = *token_from + differ;
    return to_return;
}

//strtok helper func
char * get_token(char **token_from,char * token_left)
{
    if(**token_from==0)
        return 0;
    else if(token_left)
        return abstract_token_from_source(token_from,token_left);
    else
        return return_the_last_token(token_from);
}

//strtok helper func
char * find_next_token(char ** token_from,char * token_left,const char *delim)
{
    if(!*token_from)
        return NULL;
    *token_from = *token_from + strspn(*token_from,delim);
    token_left=  strpbrk(*token_from,delim);
    return get_token(token_from,token_left);
}


char *strtok(char *str, const char *delim)
{
    static char * str_to_token;
    if(str)
        str_to_token=str;
    return find_next_token(&str_to_token,str, delim);
}


//*************************************************************

void test_strlen()
{
    require(strlen("") == 0);
    require(strlen("hello") == 5);
    require(strlen(" h ll - o ") == 10);
    printf("test_strlen okay\n");

}

void test_strcmp()
{
    char *r = "hello", *s = "hello", *t = "he";
    require(strcmp(r,s) == 0);
    require(strcmp(t,r) < 0);
    require(strcmp(s,t) > 0);
    printf("test_strcmp okay\n");
}

void test_strcpy()
{
    char src1[] = "hello", dest1[]="world",
            src2[] = "he", dest2[]="world",
            src3[] = "hello", dest3[]="wo",
            src4[] = "", dest4[]="hello",
            src5[] = "world", dest5[]=" ";

    require(strcmp(strcpy(dest1,src1),src1)==0);
    require(strcmp(strcpy(dest2,src2),src2)==0);
    require(strcmp(strcpy(dest3,src3),src3)==0);
    require(strcmp(strcpy(dest4,src4),src4)==0);
    require(strcmp(strcpy(dest5,src5),src5)==0);
    printf("test_strcpy okay\n");
}

void test_strcat()
{
    char src1[] = "hello", dest1[]="world",
            src2[] = "", dest2[]="world",
            src3[] = "hello", dest3[]="";
    require(strcmp(strcat(dest1,src1),"worldhello")==0);
    require(strcmp(strcat(dest2,src2),"world")==0);
    require(strcmp(strcat(dest3,src3),"hello")==0);
    printf("test_strcat okay\n");
}

void test_strncmp()
{
    char s11[] = "hello", s12[]="hello",
            s21[] = "hello", s22[]="heLlo",
            s31[] = "abcD", s32[]="abcd";
    require(my_strncmp(s11,s12,6)==0);
    require(my_strncmp(s21,s22,3)>0);
    require(my_strncmp(s31,s32,6)<0);
    printf("test_strncmp okay\n");
}


void test_strchr()
{
    char str[]="qiaoh3@uci.edu";
    char c1='@', c2='.', c3='i',c4=' ';
    
    require(strcmp(my_strchr(str,c1), "@uci.edu")==0);
    require(strcmp(my_strchr(str,c2), ".edu")==0);
    require(strcmp(my_strchr(str,c3), "iaoh3@uci.edu")==0);
    require(my_strchr(str,c4)==0);
    printf("test_strchr okay\n");
}

void test_strpbrk()
{
    char str[]="hwkWLiOBB6Ncafk6", s1[]="NW", s2[]="fcfee stop",s3[]="zoo";
    require(strcmp(strpbrk(str,s1), "WLiOBB6Ncafk6")==0);
    require(strcmp(strpbrk(str,s2), "cafk6")==0);
    require(strpbrk(str,s3)==0);
    printf("test_strpbrk okay\n");
}

void test_strstr()
{
    char str[]="qiaoh3@uci.edu", s1[]="h3",s2[]="i.", s3[]="i. ";

    require(strcmp(strstr(str,s1), "h3@uci.edu")==0);
    require(strcmp(strstr(str,s2), "i.edu")==0);
    require(strstr(str,s3)==0);
    printf("test_strstr okay\n");
}

void test_strspn()
{
    char str[]="qiao", s1[]="heqiao",s2[]="qiqiqiqibbbbb",s3[]="iokqiqi";

    require(strspn(s1,str)==0);
    require(strspn(s2,str)==8);
    require(strspn(s3,str)==2);
    printf("test_strspn okay\n");
}

//test_strtok() helper func
void get_tokens(char *compare_with[],char * str, char * delim)
{
    char * token = strtok(str, delim);
    while(token!=NULL)
    {
        require(strcmp(*compare_with++,token)==0);
        token=strtok(NULL, delim);
    }
    require(*compare_with==NULL);
}

void test_strtok()
{
    char s1[] = "()))require(strcmp(strcpy(dest1,src1),src1)==0)0)"
        ,s2[]="--qiao - h3@ dd- uci.edu"
        ,s3[]="2017w-cs122a-hw2-solution.pdf.www"
        ,s4[]=" ";
    char d1[] = "()"
        ,d2[]="-@"
        ,d3[]="w."
        ,d4[]=" ";
    char *r1[]={"require","strcmp","strcpy","dest1,src1",",src1","==0","0",NULL}
        ,*r2[]={"qiao "," h3", " dd", " uci.edu",NULL}
        ,*r3[]={"2017","-cs122a-h","2-solution","pdf",NULL}
        ,*r4[]={NULL};

    get_tokens(r1,s1,d1);
    get_tokens(r2,s2,d2);
    get_tokens(r3,s3,d3);
    get_tokens(r4,s4,d4);

    printf("test_strtok okay\n");
}


int main()
{
    test_strlen();
    test_strcpy();
    test_strcat();
    test_strcmp();
    test_strncmp();
    test_strchr();
    test_strpbrk();
    test_strstr();
    test_strspn();
    test_strtok();

    return 0;
}
