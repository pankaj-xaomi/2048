2048: 2048.c 2048_persist.c 2048_undo.c 2048.h
	gcc -o 2048 -Wall -Werror 2048.c 2048_persist.c 2048_undo.c -lncurses

clean: 2048
	rm -f 2048
