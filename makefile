main: main.o
	gcc -o main main.o `pkg-config --libs --cflags allegro-5`

main.o: main.c
	gcc -c main.c
