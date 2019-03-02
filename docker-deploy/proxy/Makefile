CFLAGS = -std=gnu++11 -pedantic -Wall -Werror -ggdb3 -pthread

server: main.cpp Request.o Response.o
	g++ $(CFLAGS) -o $@ $^

Request.o: Request.cpp 
	g++ $(CFLAGS) -c Request.cpp

Response.o: Response.cpp
	g++ $(CFLAGS) -c Response.cpp

.PHONY:clean
clean:
	rm -f *~ *.o *.gch




