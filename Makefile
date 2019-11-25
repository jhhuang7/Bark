FLAGS = -Wall -pedantic -std=c99 -g

bark: bark.c
	gcc $(FLAGS) bark.c -o bark
