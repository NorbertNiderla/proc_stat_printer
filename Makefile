CC=gcc
CFLAGS = -Wall -pthread

app: main.c
	$(CC) main.c -o app $(CFLAGS)

clean:
	rm ./app