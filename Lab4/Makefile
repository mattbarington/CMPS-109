#######################################################################
#
# Copyright (C) 2018 David C. Harrison. All right reserved.
#
# You may not use, distribute, publish, or modify this code without 
# the express written permission of the copyright holder.
#
#######################################################################

CFLAGS = -Wall -Iinclude
CC = g++ -std=c++17

SRC=$(wildcard src/*.cc)

bounds: $(SRC) 
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) 2>&1 | tee make.out

check: bounds
	@./check.sh
	@echo ""

grade: clean bounds
	@./grade.sh 

comply: 
	@./comply.sh

clean:
	@rm -f bounds make.out

submit: clean comply
	@tar czvf ~/CMPS109-Lab4.tar.gz --exclude include/containable.h src/*.cc include/*.h 
