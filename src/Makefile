
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

all: libsift.o libmatch.o libgeometry.o util.o directory.o
	mpic++ dosift.cpp util.o directory.o libsift.o $(OPTS) -o ulsift -I../include/ -L../lib/ $(LIBS)
	mpic++ domatch.cpp util.o directory.o libsift.o libmatch.o libgeometry.o $(OPTS) -o ulmatch -I../include/ -L../lib/ $(LIBS)

util.o: util.cpp util.h
	$(CXX) $(OPTS) -c util.cpp -o util.o -I../include/ $(OPENCV)

directory.o: directory.cpp directory.h
	$(CXX) $(OPTS) -c directory.cpp -o directory.o -I../include/ $(OPENCV)

libsift.o: libsift.cpp libsift.h
	$(CXX) $(OPTS) -c libsift.cpp -o libsift.o -I../include/ $(OPENCV)

libmatch.o: libmatch.cpp libmatch.h
	$(CXX) $(OPTS) -c libmatch.cpp -o libsift.o -I../include/ $(OPENCV)

libgeometry.o: libgeometry.cpp libgeometry.h
	$(CXX) $(OPTS) -c libgeometry.cpp -o libsift.o -I../include/ $(OPENCV)

clean:
	rm -rf *.o
	rm -f ulsift

install:
	chmod +x ulsift
	cp ulsift ../bin/
	cp ulsift /usr/local/bin/
	chmod +x ulmatch
	cp ulmatch ../bin/
	cp ulmatch /usr/local/bin/

cinstall:
	chmod +x ulsift
	cp ulsift ../bin/
	chmod +x ulmatch
	cp ulmatch ../bin/

test:
	chmod +x ulsift
	cp ulsift ../bin/
	chmod +x ulmatch
	cp ulmatch ../bin/
