MUDUO_DIRECTORY ?= $(PWD)/muduo
MUDUO_INCLUDE = $(MUDUO_DIRECTORY)/include
MUDUO_LIBRARY = $(MUDUO_DIRECTORY)/lib
MUDUO_CODEC_PATH =  $(MUDUO_DIRECTORY)/codec
MUDUO_PUBSUB_PATH =  $(MUDUO_DIRECTORY)/pubsub


CXXFLAGS = -g -O0 -Wall -Wextra \
	   -Wconversion -Wno-unused-parameter \
	   -Wold-style-cast -Woverloaded-virtual \
	   -Wpointer-arith -Wshadow -Wwrite-strings \
	   -march=native -rdynamic \
	   -I$(MUDUO_INCLUDE) -I$(MUDUO_CODEC_PATH) -I$(MUDUO_PUBSUB_PATH)

LDFLAGS = -L$(MUDUO_LIBRARY)  -lmuduo_net -lmuduo_base -lpthread -lrt 

all: client server

clean:
	rm -f client server

client: src/client/client.cc muduo/codec/codec.cc muduo/pubsub/pubsub.cc
	g++ $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

server: src/server/server.cc muduo/codec/codec.cc muduo/pubsub/pubsub.cc
	g++ $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: all clean
