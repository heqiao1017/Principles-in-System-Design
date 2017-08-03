all: handle_signals my_shell
handle_signals: handle_signals.o
	gcc -ggdb -std=gnu99 handle_signals.o -o handle_signals
handle_signals.o: handle_signals.c
	gcc -ggdb -c handle_signals.c
my_shell: my_shell.o
	gcc -ggdb -std=c99 my_shell.o -o my_shell
my_shell.o: my_shell.c
	gcc -ggdb -std=c99 -c my_shell.c
clean:
	/bin/rm -f handle_signals.o handle_signals my_shell.o my_shell
