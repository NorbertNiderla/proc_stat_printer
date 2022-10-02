CC=gcc
CFLAGS = -Wall -Wextra -pthread

app: main.c
	$(CC) main.c -o app $(CFLAGS)

clean:
	rm ./app

test_ringbuffer: ringbuffer.c test_ringbuffer.c
	$(CC) test_ringbuffer.c ringbuffer.c -o test_app $(CFLAGS)
	./test_app
	rm test_app

test_proc_stat: proc_stat.c test_proc_stat.c
	$(CC) test_proc_stat.c proc_stat.c -o test_app $(CFLAGS)
	./test_app
	rm test_app