all: server

clean:
	@rm -rf *.o
	@rm -rf server

server: main.o server.o request.o header.o utils.o response.o
	gcc -g3 -fsanitize=address,undefined -o server $^

main.o: main.c
	gcc -g3 -fsanitize=address,undefined -c -o main.o main.c

server.o: server.c server.h
	gcc -g3 -fsanitize=address,undefined -c -o server.o server.c

request.o: request.c request.h
	gcc -g3 -fsanitize=address,undefined -c -o request.o request.c

response.o: response.c response.h
	gcc -g3 -fsanitize=address,undefined -c -o response.o response.c

header.o: header.c header.h
	gcc -g3 -fsanitize=address,undefined -c -o header.o header.c

utils.o: utils.c utils.h
	gcc -g3 -fsanitize=address,undefined -c -o utils.o utils.c
