CC=gcc
CFLAGS = -Wall -pthread

app: main.c
	$(CC) main.c -o app $(CFLAGS)

clean:
	rm ./app

test_ringbuffer: ringbuffer.c test_ringbuffer.c
	$(CC) test_ringbuffer.c ringbuffer.c -o test_app $(CFLAGS)
	./test_app
	rm test_app