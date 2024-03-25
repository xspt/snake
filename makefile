main: main.o snake.o
	gcc -o main main.o snake.o `pkg-config --libs --cflags allegro-5`

main.o: main.c
	gcc -c main.c

snake.o: snake.c
	gcc -c snake.c
