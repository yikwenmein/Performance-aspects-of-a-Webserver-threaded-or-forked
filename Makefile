TARGET=serverthread serverfork
CC=gcc
CFLAGS= -Wall -Wextra -g
normal: $(TARGET)
serverthread: serverthread.c
	$(CC) $(CFLAGS) serverthread.c -o serverthread -pthread
serverfork: serverfork.c
	$(CC) $(CFLAGS) serverfork.c -o serverfork
clean:
	$(RM) serverthread serverfork
