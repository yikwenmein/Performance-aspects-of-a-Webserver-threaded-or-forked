TARGET=serverthread
CC=gcc
CFLAGS= -Wall -Wextra -g
normal: $(TARGET)
serverthread: serverthread.c
	$(CC) $(CFLAGS) serverthread.c -o serverthread -pthread
clean:
	$(RM) serverthread
