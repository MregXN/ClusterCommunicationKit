MUDUO_DIRECTORY ?= $(PWD)/muduo
MUDUO_INCLUDE = $(MUDUO_DIRECTORY)/include
MUDUO_LIBRARY = $(MUDUO_DIRECTORY)/lib
MUDUO_CODEC = $(MUDUO_DIRECTORY)/codec

CXXFLAGS = -g -O0 -Wall -Wextra \
	   -Wconversion -Wno-unused-parameter \
	   -Wold-style-cast -Woverloaded-virtual \
	   -Wpointer-arith -Wshadow -Wwrite-strings \
	   -march=native -rdynamic \
	   -I$(MUDUO_INCLUDE)   -I$(MUDUO_CODEC) 

LDFLAGS = -L$(MUDUO_LIBRARY)  -lmuduo_net -lmuduo_base -lpthread -lrt 

all: client server

clean:
	rm -f client server

client: src/client/main.cc src/client/client.cc muduo/codec/codec.cc 
	g++ $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

server: src/server/main.cc src/server/server.cc muduo/codec/codec.cc 
	g++ $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: all clean
