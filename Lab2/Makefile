#######################################################################
#
# Copyright (C) 2018 David C. Harrison. All right reserved.
#
# You may not use, distribute, publish, or modify this code without 
# the express written permission of the copyright holder.
#
#######################################################################

include Makefile.libs

CFLAGS = -Wall
CC = gcc

SRC=$(wildcard *.c)

bounds: $(SRC) test.o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) 2>&1 | tee make.out

check: bounds
	@./check.sh
	@echo ""

grade: clean bounds
	@./grade.sh 

clean:
	@rm -f bounds make.out

submit: clean
	@tar czvf ~/CMPS109-Lab2.tar.gz \
	--exclude=bounds.h Makefile.libs *.c *.h *.test
