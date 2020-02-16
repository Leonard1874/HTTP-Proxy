SOURCES_M=ringmaster.cpp
OBJS_M=$(patsubst %.cpp, %.o, $(SOURCES_M))
SOURCES_P=client.cpp
OBJS_P=$(patsubst %.cpp, %.o, $(SOURCES_P))
CPPFLAGS=-ggdb3 -Wall -Werror -pedantic -std=gnu++11

client: $(OBJS_P)
	g++ $(CPPFLAGS) -o client $(OBJS_P)
clean:
	rm test *~ *.o
