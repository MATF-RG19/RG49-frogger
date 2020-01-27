CC=g++
frogger: main.o image.o
	$(CC) -o frogger main.o image.o -lGL -lGLU -lglut -lm -Wall -Wextra
main.o: main.cpp
	$(CC) -c main.cpp
