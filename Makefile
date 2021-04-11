TARGET=proj2

default: all

all:
	gcc $(TARGET).c -std=gnu99 -Wall -Wextra -Werror -pedantic -o $(TARGET) 

run: all
	./$(TARGET) 