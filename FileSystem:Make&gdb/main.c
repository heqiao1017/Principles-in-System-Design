#include "my_ls.h"

int main(int argc, char *argv[])
{
    if(argc==1)
        do_ls(".");
    else
        for(int i=1;i<argc;i++)
            do_ls(argv[i]);
    
    return 0;
}
