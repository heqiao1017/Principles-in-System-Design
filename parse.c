#include <stdio.h>
#include <ctype.h>
#define LEXEMESIZE 100
char lexeme[LEXEMESIZE];
int lexeme_len=0;

void clear_lexeme()
{
    lexeme[0]=0;
    lexeme_len=0;
}

int peekc()
{
    int c=getchar();
    ungetc(c,stdin);
    return c;
}

void print_to_output()
{
    lexeme[lexeme_len]=0;
    printf("%s\n",lexeme);
    clear_lexeme();
}


void parse_error(char * message, char ch)
{
    fprintf(stderr, "Error: %s: %c\n", message, ch);
}
 
void parseNumber(char c)
{
    lexeme[lexeme_len++]=c;
    while(isdigit(peekc()))
        lexeme[lexeme_len++]=getchar();
    print_to_output();
}

void parseWord(char c)
{
    lexeme[lexeme_len++]=c;
    while(isalnum(peekc()) || peekc()=='_')
        lexeme[lexeme_len++]=getchar();
    print_to_output();
}

void parse_literal(char c, char mode)
{
    lexeme[lexeme_len++]=c;
    while(peekc()!=mode || lexeme[lexeme_len-1]=='\\')
        lexeme[lexeme_len++]=getchar();
    lexeme[lexeme_len++]=getchar();
    print_to_output();
}

void parse_double_operator(char c)
{
    
    if((c=='+' && peekc() == '+')
       || (c=='-' && peekc() == '-')
       || (c=='=' && peekc() == '=')
       || (c=='&' && peekc() == '&')
       || (c=='|' && peekc() == '|')
       || (c=='>' && peekc() == '=')
       || (c=='!' && peekc() == '=')
       || (c=='<' && peekc() == '=')
       || (c=='+' && peekc() == '=')
       || (c=='-' && peekc() == '=')
       || (c=='*' && peekc() == '=')
       || (c=='/' && peekc() == '=')
       || (c=='%' && peekc() == '='))
    {
        lexeme[lexeme_len++]=getchar();
    }
}

void ignore_comment(char c)
{
    while((c=getchar()) != '*' || peekc()!='/')
        ;
    getchar();

//    if(peekc()=='/')
//    {
//        while((c=getchar())!='\n');
//    }else if(peekc()=='*'){
//        while((c=getchar()) != '*' || peekc()!='/');
//        getchar();
//    }
}

void parse_operator(char c)
{
    lexeme[lexeme_len++]=c;
    parse_double_operator(c);
    print_to_output();
}

void parse_word_or_num(char c)
{
    if(isdigit(c))
        parseNumber(c);
    else if(isalpha(c) || c=='_')
        parseWord(c);
    else
        parse_error("Illegal character %c\n", c);
}

void getToken(char c)
{
    switch(c)
    {
        case ' ':
        case '\t':
        case '\n':
            break;
        case '/':
            if(peekc()=='*')
                ignore_comment(c);
            else
                parse_operator(c);
            break;
        case '&':
        case '|':
        case '<':
        case '>':
        case '#':
        case '.':
        case ';':
        case '*':
        case ',':
        case '(':
        case ')':
        case '{':
        case '}':
        case '[':
        case ']':
        case '=':
        case '+':
        case '-':
        case '!':
        case ':':
        case '%':
            parse_operator(c);
            break;
        case '"':
            parse_literal(c,'"');
            break;
        case '\'':
            parse_literal(c,'\'');
            break;
        default:
            parse_word_or_num(c);
            break;
    }
}
            

int main()
{
    char c;
    while((c=getchar()) != EOF){
        getToken(c);
    }
    return 0;
}

