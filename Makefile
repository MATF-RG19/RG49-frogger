CC=g++
zaba: main.o
	$(CC) -o zaba main.o  -lGL -lGLU -lglut -lm -Wall -Wextra
main.o: main.cpp
	$(CC) -c main.cpp
