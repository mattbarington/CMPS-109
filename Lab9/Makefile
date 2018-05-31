#######################################################################
#
# Copyright (C) 2018 David C. Harrison. All right reserved.
#
# You may not use, distribute, publish, or modify this code without
# the express written permission of the copyright holder.
#
#######################################################################

CFLAGS = -Wall -Isrc -lpthread -lcrypt -g
CC = g++ -std=c++17

SRC=$(wildcard src/*.cc)

SERVER_SRC = src/server.cc src/crack.h
CRACKER_SRC = src/cracker.cc src/crack.h

all: cracker server

cracker: $(CRACKER_SRC) crack.o
	$(CC) -o $@ $^ $(CFLAGS)

server: $(SERVER_SRC) crack.o
	$(CC) -o $@ $^ $(CFLAGS)

# crack: src/crack.cc crack.o src/crack.h
# 	g++ -std=c++17 -o crack $(CFLAGS)

clean:
	@rm cracker server

submit: clean
	@tar czvf ~/CMPS109-Lab9.tar.gz \
	    src/*.cc src/*.h
