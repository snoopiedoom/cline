all: main.c
	clang -Wall -o cline main.c -lncurses
