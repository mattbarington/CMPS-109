#######################################################################
#
# Copyright (C) 2018 David C. Harrison. All right reserved.
#
# You may not use, distribute, publish, or modify this code without 
# the express written permission of the copyright holder.
#
#######################################################################

CFLAGS = -Wall -Isrc -lpthread
CC = g++ -std=c++17

SRC=$(wildcard src/*.cc)

all: radix mperf

radix: $(SRC) main.o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) 2>&1 | tee make.out

mperf: $(SRC) perf.o
	$(CC) -o perf $^ $(CFLAGS) $(LIBS) 2>&1 >/dev/null 

check: radix
	@./check.sh 500000

perf: mperf
	@./perf.sh  500000

grade: clean radix mperf
	@./grade.sh 500000

clean:
	@rm -f radix perf make.out

submit: clean 
	@tar czvf ~/CMPS109-Lab5.tar.gz \
	    --exclude Makefile.h \
	    src/*.cc src/*.h 
