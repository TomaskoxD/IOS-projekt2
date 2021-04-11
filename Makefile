TARGET=proj2

default: all

all:
	gcc $(TARGET).c -Wall -Wextra -o $(TARGET) 

run: all
	./$(TARGET) 