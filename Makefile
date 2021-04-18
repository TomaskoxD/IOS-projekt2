TARGET=proj2
CFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic
default: all

all:
	gcc $(CFLAGS) $(TARGET).c -o $(TARGET) -lrt -pthread

clean:
	rm -f proj2 proj2.out proj2.zip

zip:
	zip -r proj2.zip proj2.c Makefile

run: all
<<<<<<< HEAD
	./$(TARGET) 5 5 900 900
=======
	./$(TARGET) 10 10 999 999
>>>>>>> 5b996bb20a808e8b598cd5198915c90ebf12fef8
