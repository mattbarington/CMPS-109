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

radix: $(SRC) main.o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) 2>&1 | tee make.out

check: radix
	@./check.sh 99999
	@echo ""

perf: radix
	@./perf.sh  99999
	@echo ""

grade: clean radix
	@./grade.sh 99999

clean:
	@rm -f radix make.out

submit: clean 
	@tar czvf ~/CMPS109-Lab5.tar.gz \
	    --exclude Makefile.h \
	    src/*.cc src/*.h 
