CC=g++
zaba: main.o
	$(CC) -o zaba main.o  -lGL -lGLU -lglut -lm
main.o: main.c
	$(CC) -c main.c
