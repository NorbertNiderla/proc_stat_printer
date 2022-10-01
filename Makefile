CC=gcc

app: main.c
	$(CC) main.c -o app

clean:
	rm ./app