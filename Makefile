TARGET=proj2
CFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic
default: all

all:
	gcc $(CFLAGS) $(TARGET).c -o $(TARGET) -lrt -pthread

clean:
	rm -f proj2 proj2.out

run: all
	./$(TARGET) 2 2 100 100