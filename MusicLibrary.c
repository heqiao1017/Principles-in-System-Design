/*
ICS 53
HW 1
Write an ANSI C program, called MusicLibrary, that allows the user to manage their music collection.  Each MusicLibrary has a name specified as an argument to the command line.  If none is given, use the default name of myMusic.  When the program is run, the named MusicLibrary is loaded from the file (name of file matches name of MusicLibrary) into memory.  If the file does not exist, the in-memory MusicLibrary will be empty.  When the program is exited, the MusicLibrary is saved back to the file from which it came. Put your entire program in one file named MusicLibrary.c. You will compile your program with the command gcc MusicLibrary.c -o MusicLibrary

A MusicLibrary is a list of songs - each of which has a title, artist, and year published.  Note we may add more fields in the future, but for now, just store those three values.  Songs are retrieved or deleted by using their title. You may limit each title and artist to 40 characters.

Your main program will be a loop that prints a prompt “myMuisc Command: “ where myMusic will be the name of the current MusicLibrary.  Each command is a single letter and either upper or lower case letters are treated the same.  Any invalid command character is skipped and ignored.
Implement the following additional commands:

I - insert a new song, prompt for title, artist, and year published, maintain the list in ascending alphabetical order by song title, ignore letter case when doing comparisons, but retain letter case in all stored information
P - print out all the entries in this MusicLibrary
D - delete a specified song, prompt for the title of the song to be deleted, delete all matching songs
L - lookup the song for a specified title, prompt for the title to look-up, use binary search which only works on sorted list.
Q - save the current MusicLibrary to its file and exit (quit) the program.

Here is a sample session starting with myMusic. “bash $” is the Linux command shell prompt, so I am running the program in the first command and after the last Q, my program has exited and I am back at the command prompt where I give a command to cat myMusic so you can see the three entries as they are stored in the file myMusic.  Note, you may store the entries in the file in your choice of format.
bash $ MusicLibrary
i
Title: C my title
Artist: C my artist
Year Published: 2014
i
Title: A my title
Artist: A my artist
Year Published: 2014
i
Title: B my title
Artist: B my artist
Year Published: 2014
p
1 Title: A my title, Artist: A my artist, Year Published: 2014
2 Title: B my title, Artist: B my artist, Year Published: 2014
3 Title: C my title, Artist: C my artist, Year Published: 2014
L
Title: B my title
Title: B my title, Artist: B my artist, Year Published: 2014
D
Title: B my title
P
1 Title: A my title, Artist: A my artist, Year Published: 2014
2 Title: C my title, Artist: C my artist, Year Published: 2014
Q
bash $ cat myMusic
Title: A my title, Artist: A my artist, Year Published: 2014
Title: C my title, Artist: C my artist, Year Published: 2014

Design Sketch This is how I structured my program and you must follow my design for your program to help you get used to identifying useful functions as you write programs.  You must keep your functions reasonably small so your program is understandable - no function body over 5 lines, except switch statement.  Large functions quickly become difficult to read, understand, and debug.  Lines with a single curly brace do not count towards the maximum of 5 lines nor does the function prototype count.  Variable declarations do not count either and you may declare multiple variables of the same type in one line, e.g., int i = 9, j = 0;

Remember that C is a one-pass language, so items must be declared BEFORE they are referenced (called).  It often works best to write programs upside-down with main at the bottom of the file, then above it are the functions main calls, then above them are the functions they call, and so on.  Otherwise, you will have to repeat the function declaration above their first use.  Having the same information in two places makes it difficult to change things while you program.

Write your entire program in one file.  At the top of your file, declare the structure for a Song which contains data members for each item you plan to store in a MusicLibrary entry. 
#define STRMAX 100
struct Song {
char title[STRMAX];
char artist[STRMAX];
int year_published;
};

Use an array of Songs for your MusicLibrary. You should pick an upper bound on the size (1024 is fine) and declare that as a symbolic constant defined with #define, then you must declare your array of Songs of the appropriate size.  You must keep a count of how many songs are currently in the MusicLibrary so you know how many are in the array and where the next available array slot is located.
#define SONGMAX 1024
struct Song music_library[SONGMAX];
int current_number_of_songs = 0;

You must include stdio.h so you can do file I/O. If you call exit(), you must include stdlib.h.  You may want to include string.h so you can use strcmp() to compare two C strings. Example of strcmp You will use strcpy() to assign characters from one char array to another.

Start with a main procedure (at the bottom of the file).  It can determine the name of the MusicLibrary file (defaulting to “myMusic”), then load the saved data into the memory structure, Then it can loop reading a user command then evaluating that command.  When that terminates, it can save the MusicLibrary back to the file.  You may declare your song array local to main if you prefer, but you must pass the array into each function that is called.  For this program, it is fine to declare the song array as a top-level (global) variable.

If you have experience with object-oriented programming languages, think of this program as a class that has data members for the array and the count of songs and the functions you write are methods on the class.

read_command can print a prompt, then read a character from the user using getchar().  Skip any whitespace characters (space, tab, newline).

evaluate_command can take a command character and decide which command it is, then do the appropriate action.

load_MusicLibrary and store_MusicLibrary load or store a named MusicLibrary file into the memory MusicLibrary. Refer to the lecture notes “Formatted File I/O” for an example of reading/writeing from/to files Link

I wrote a command print_MusicLibrary that may be used for both storing the MusicLibrary to a file or for printing the MusicLibrary on the terminal for the P command.  I passed in a boolean argument to indicate if I want line numbers (for terminal print) or not (for file print). 

find_index_of_song_with_name returns the location of the song with the specified name (used by remove and lookup). This is the function that should do binary search and return the index of where it found the song or where the song should be if it were in this array list.

remove_song_from_MusicLibrary_by_name removes a song with the specified name. You will find it with int i = find_index_of_song_with_name(), then crunch_up_from_index(i).

add_song_to_MusicLibrary takes a song and puts it in the MusicLibrary in memory in the proper location. You could use i = find_index_of_song_with_name(...), then crunch_down_from_index(i), then buf[i] = song.

write_song and read_song handle writing and reading songs from a specified file.

open_file takes a specified file name and a file mode, either “r” or “w” and opens that file, does error checking, then returns the file handle (FILE *).

*/


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h>

#define STRMAX 40
#define SONGMAX 1024
#define BUFFMAX 200

struct Song {
    char title[STRMAX];
    char artist[STRMAX];
    int year_published;
};

struct Song music_library[SONGMAX];
int current_number_of_songs = 0;

void open_file_error_checking(FILE * file_open, const char * filename, char mode)
{
    if(file_open == NULL && mode=='w'){
        fprintf(stderr, "Can't open %s\n", filename);
        exit(1);
    }
}

FILE * choose_mode(char mode, const char * filename)
{
    if(mode == 'r')
        return fopen(filename,"r");
    else
        return fopen(filename,"w");
}
FILE * open_file(const char * filename, char mode)
{
    FILE* file_open = choose_mode(mode,filename);

    open_file_error_checking(file_open, filename, mode);

    return file_open;
}

void crunch_down_from_index(int i)
{
    for(int j=current_number_of_songs-1;j>=i; --j)
        music_library[j+1]=music_library[j];
}

void crunch_up_from_index(int i)
{
    for(int j=i;j<current_number_of_songs; ++j)
        music_library[j]=music_library[j+1];
}


void continue_compare_middle_index_title_with_song_name(int cmp_result, char * name, int * middle, int * begin, int * end)
{
    if(cmp_result<0)
        *begin = *middle+1;
    else
        *end = *middle-1;
    *middle = (*begin+*end)/2;
}
void binary_search_index_with_song_name(int cmp_result, int * already_in_array_index, char * name, int * begin, int end, int middle)
{
    while(*begin<=end)
    {
        cmp_result = strcasecmp(music_library[middle].title,name);

        if(cmp_result==0)
            *already_in_array_index = middle;

        continue_compare_middle_index_title_with_song_name(cmp_result, name, &middle, begin, &end);
    }
}

int find_index_of_song_with_name(char * name)
{
    int begin=0, end=current_number_of_songs-1, middle=(begin+end)/2, cmp_result, already_in_array_index = -1;

    binary_search_index_with_song_name(cmp_result, &already_in_array_index, name, &begin, end, middle);

    if(already_in_array_index==-1)
        return begin;

    return already_in_array_index;
}


void remove_song_from_MusicLibrary_by_index(int index_to_remove)
{
    crunch_up_from_index(index_to_remove);
    --current_number_of_songs;
}

void remove_song_from_MusicLibrary_by_name(char * name)
{
    int index_to_remove = find_index_of_song_with_name(name);
    int cmp_result = strcasecmp(name, music_library[index_to_remove].title);
    
    if(cmp_result==0)
        remove_song_from_MusicLibrary_by_index(index_to_remove);
    else
        printf("No matching song found to delete\n");
}


void add_song_to_MusicLibrary(struct Song song_to_add)
{
    int index_to_add = find_index_of_song_with_name(song_to_add.title);
    crunch_down_from_index(index_to_add);
    music_library[index_to_add] = song_to_add;
    ++current_number_of_songs;
}
void write_song(const char * filename)
{
    FILE * outputfile = open_file(filename, 'w');

    for(int i=0 ;i <current_number_of_songs ; ++i)
        fprintf(outputfile, "Title:%s, Artist:%s, Year Published:%d\n", music_library[i].title, music_library[i]. artist, music_library[i].year_published);

    fclose(outputfile);
}

void printing_on_terminal()
{
    for(int i=0 ;i < current_number_of_songs ; ++i)
        printf("%d Title:%s, Artist:%s, Year Published:%d\n", i+1 ,music_library[i].title, music_library[i]. artist, music_library[i].year_published);
}


void print_MusicLibrary(bool want_line_number, const char * filename)
{
    if(!want_line_number)
        write_song(filename);
    else
        printing_on_terminal();
}

void lookup_song_with_name(char * name)
{
    int index_to_lookup = find_index_of_song_with_name(name);
    int cmp_result = strcasecmp(name,music_library[index_to_lookup].title);

    if(cmp_result == 0)
        printf("Title:%s, Artist:%s, Year Published:%d\n", music_library[index_to_lookup].title, music_library[index_to_lookup]. artist, music_library[index_to_lookup].year_published);
    else
        printf("No matching song found to lookup\n");
}

void store_MusicLibrary(const char * filename)
{
    print_MusicLibrary(false, filename);
}



void convert_buffer_input_to_song_attribute(struct Song * new_song, int buff_index, char * buff_splitted)
{

    switch(buff_index)
    {
        case 1:
            strcpy((*new_song).title,buff_splitted);
            break;
        case 3:
            strcpy((*new_song).artist,buff_splitted);
            break;
        case 5:
            (*new_song).year_published  = atoi(buff_splitted);
            break;
        default:
            break;
    }

}

void parse_buffer_content(struct Song * song_to_add, char * buff)
{

    char * buff_splitted = strtok (buff, ",:");
    for(int j=0; buff_splitted !=NULL; ++j)
    {
        convert_buffer_input_to_song_attribute(song_to_add, j , buff_splitted);
        buff_splitted = strtok(NULL, ",:");
    }
}

void read_song(FILE * inputFile)
{
    char buff[BUFFMAX];
    struct Song song_to_add;

    while(fgets(buff, BUFFMAX, (FILE*)inputFile)!=NULL)
    {
        buff[strlen(buff)-1] = '\0';  //get rid of the newline character
        parse_buffer_content(&song_to_add, buff);
        add_song_to_MusicLibrary(song_to_add);
    }

}


void load_MusicLibrary(const char * filename)
{
    FILE * readfile = open_file(filename, 'r');

    if(readfile!=NULL)
    {
        read_song(readfile);
        fclose(readfile);
    }
}

int prompt_for_int_input(char * prompt)
{
    int input_num;
    printf("%s:", prompt);
    scanf("%d", &input_num);
    while (getchar() != '\n');
    return input_num;
}
void get_input_line(char * buff)
{
    fgets(buff, STRMAX, stdin);
    int length = strlen(buff)-1;
    if(buff[length] == '\n')
        buff[length]='\0';
}

void prompt_for_string_input(char* input_str, char * prompt)
{
    char buff[STRMAX];
    printf("%s:", prompt);
    get_input_line(buff);
    strcpy(input_str,buff);
}

void copy_input_to_song(struct Song * song_to_add, char* name, char* author, int year)
{
    strcpy((*song_to_add).title, name );
    strcpy((*song_to_add).artist, author);
    (*song_to_add).year_published = year;
}

void get_song_info(char * name, char *author, int* year)
{
    prompt_for_string_input(name, "Title");
    prompt_for_string_input(author, "Artist");
    *year = prompt_for_int_input("Year Published");
}

struct Song prompt_command_for_song_to_add()
{
    struct Song song_to_add;
    char name[STRMAX], author[STRMAX];
    int year;

    get_song_info(name, author, &year);
    copy_input_to_song(&song_to_add, name, author, year);
    return song_to_add;
}

void evaluate_command(char command, const char * filename)
{
    struct Song new_song;
    char input_song_name[STRMAX];

    switch(command)
    {
        case 'i':
        case 'I':
            new_song = prompt_command_for_song_to_add();
            add_song_to_MusicLibrary(new_song);
            break;
        case 'p':
        case 'P':
            print_MusicLibrary(true, filename);
            break;
        case 'd':
        case 'D':
            prompt_for_string_input(input_song_name, "Title");
            remove_song_from_MusicLibrary_by_name(input_song_name);
            break;
        case 'l':
        case 'L':
            prompt_for_string_input(input_song_name, "Title");
            lookup_song_with_name(input_song_name);
            break;
        case 'q':
        case 'Q':
            store_MusicLibrary(filename);
            exit(0);
        default:
            break;
    }
}

char read_command(const char * filename)
{
    char command;
    printf("%s Command: ", filename);
    command = getchar();
    while(command!='\n' && getchar()!='\n');
    return command;
}

void loop_through_commands(const char * filename)
{
    char command;

    do{
        command = read_command(filename);
        evaluate_command(command, filename);
    }while(tolower(command) != 'q');

}


char * determine_filename(int argc, char * argv[])
{
    if(argc==1)
        return "myMusic";
    else
        return argv[1];
}

int main(int argc, char * argv[])
{
    char * filename = determine_filename(argc, argv);
    load_MusicLibrary(filename);
    loop_through_commands(filename);
    return 0;
}



