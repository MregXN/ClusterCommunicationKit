MUDUO_DIRECTORY ?= $(PWD)/muduo
MUDUO_INCLUDE = $(MUDUO_DIRECTORY)/include
MUDUO_LIBRARY = $(MUDUO_DIRECTORY)/lib
MUDUO_CODEC = $(MUDUO_DIRECTORY)/codec
MUDUO_SRC = $(MUDUO_DIRECTORY)/../src

CXXFLAGS = -g -O0 -Wall -Wextra \
	   -Wconversion -Wno-unused-parameter \
	   -Wold-style-cast -Woverloaded-virtual \
	   -Wpointer-arith -Wshadow -Wwrite-strings \
	   -march=native -rdynamic   \
	   -I$(MUDUO_INCLUDE)   -I$(MUDUO_CODEC)  -I$(MUDUO_SRC) 

LDFLAGS = -L$(MUDUO_LIBRARY)  -lmuduo_net -lmuduo_base -lmuduo_http -lpthread -lrt 

all: client server

clean:
	rm -f client server

client: src/client/main.cc src/client/client.cc muduo/codec/codec.cc src/getinfo.cc
	g++ $(CXXFLAGS) -o $@ $^ $(LDFLAGS) -std=c++11

server: src/server/main.cc src/server/server.cc muduo/codec/codec.cc src/getinfo.cc
	g++ $(CXXFLAGS) -o $@ $^ $(LDFLAGS) -std=c++11 

.PHONY: all clean
