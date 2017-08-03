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



