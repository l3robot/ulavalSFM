
# File : src/Makefile
# Author : Émile Robitaille @ LERobot
# Last update : 07/02/2014
# Version : 1.0
# Description : Makefile used to compile ulavalSFM.

LIBS=$(shell pkg-config --cflags --libs opencv)
OPENCV=$(shell pkg-config --cflags opencv)

OPTS=-gdwarf-2 --std=c++11

.PHONY: clean, install, cinstall, test

.SUFFIXES:

all: libsift.o util.o directory.o
	mpic++ dosift.cpp util.o directory.o libsift.o $(OPTS) -o ulsift -I../include/ -L../lib/ $(LIBS)

util.o: util.cpp util.h
	$(CXX) $(OPTS) -c util.cpp -o util.o -I../include/ $(OPENCV)

directory.o: directory.cpp directory.h
	$(CXX) $(OPTS) -c directory.cpp -o directory.o -I../include/ $(OPENCV)

libsift.o: libsift.cpp libsift.h
	$(CXX) $(OPTS) -c libsift.cpp -o libsift.o -I../include/ $(OPENCV)

clean:
	rm -rf *.o
	rm -f ulsift

install:
	chmod +x ulsift
	cp ulsift ../bin/
	cp ulsift /usr/local/bin/

cinstall:
	cp ulsift ../bin/

test:
	chmod +x ulsift
	cp ulsift ../bin/
