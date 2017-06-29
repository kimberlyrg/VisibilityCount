viscalc: viscalc.c C-Thread-Pool/thpool.h C-Thread-Pool/thpool.c
	gcc -g viscalc.c C-Thread-Pool/thpool.c -lpthread -lm -o viscalc -Wall
