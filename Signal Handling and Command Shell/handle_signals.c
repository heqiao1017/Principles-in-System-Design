/*
 Signal Handling. Write a C program, called handle_signals, to do the following:
 continually print X's on the screen using the following main program:
 int main() {
 // Turn off output buffering here if you like
 init_signal_handlers();
 while (1) {
 // Print an 'X' and flush the output buffer here
 sleep(1); // sleeps this thread for one second
 }
 }
 whenever the user types a ^C (interrupt) print an I, flush, DO NOT abort
 whenever the user types a ^\ (quit) print a Q, flush, DO NOT quit
 whenever the user types a ^Z (terminal stop) print an S, flush, then stop. You, the user, may resume the job with the fg command from the shell.  On the third ^Z, print a summary of the number of signals received as follows, then exit:
 Interrupt: 5
 Stop: 3
 Quit: 2
 You should flush the output after printing each character to prevent problems with buffered output or just turn off output buffering - your choice. Also, you may use signal() or sigaction(), but sigaction() is the preferred way to handle signals as it is the new and improved interface.  Here is a link with an intro to signal handling: http://www.alexonlinux.com/signal-handling-in-linux
*/



#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

struct sigaction act;

int interrupt_count=0, stop_count=0,quit_count=0;

void stop()
{
    if(++stop_count>=3){
        printf("\nInterrupt: %d\nStop: %d\nQuit: %d\n",interrupt_count, stop_count,quit_count);
        exit(0);
    }else
        raise(SIGSTOP);
}

void sighandler(int signum, siginfo_t *info, void *ptr)
{
    switch(signum)
    {
        case SIGINT:
            printf("I");
            fflush(stdout);
            interrupt_count++;
            break;
        case SIGQUIT:
            printf("Q");
            fflush(stdout);
            quit_count++;
            break;
        case SIGTSTP:
            printf("S");
            fflush(stdout);
            stop();
            break;  
        default:
            break;
    }

}

void examine_signal()
{
    sigaction(SIGINT, &act, NULL);
    sigaction(SIGQUIT, &act, NULL);
    sigaction(SIGTSTP, &act, NULL);
}

void init_signal_handlers()
{
    act.sa_sigaction = sighandler;
    act.sa_flags = SA_SIGINFO;
    examine_signal();
}


int main() {

    init_signal_handlers();

    while(1)
    {
        printf("X");
        fflush(stdout);
        sleep(1);
    }

    return 0;
}

