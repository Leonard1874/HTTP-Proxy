SOURCES_M=host.cpp
OBJS_M=$(patsubst %.cpp, %.o, $(SOURCES_M))
SOURCES_P=client.cpp
OBJS_P=$(patsubst %.cpp, %.o, $(SOURCES_P))
CPPFLAGS=-ggdb3 -Wall -Werror -pedantic -std=gnu++11

all: host client

host: $(OBJS_M)
	g++ $(CPPFLAGS) -o host $(OBJS_M)
client: $(OBJS_P)
	g++ $(CPPFLAGS) -o client $(OBJS_P)
clean:
	rm  *~ *.o
