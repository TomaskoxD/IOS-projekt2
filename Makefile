TARGET=proj2
CFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic
default: all

all:
	gcc $(CFLAGS) $(TARGET).c -o $(TARGET) -lrt -pthread

clean:
	rm -rf proj2 proj2.out proj2.zip

zip:
	zip -r proj2.zip proj2.c Makefile

run: clean all
	./$(TARGET) 7 8 900 900 && sleep 1 && cat proj2.out
