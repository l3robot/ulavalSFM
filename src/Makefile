
# File : src/Makefile
# Author : Émile Robitaille @ LERobot
# Last update : 07/02/2014
# Version : 1.0
# Description : Makefile used to compile ulavalSFM.

#CXX=g++
LIBS=$(shell pkg-config --cflags --libs opencv)
OPENCV=$(shell pkg-config --cflags opencv)

.PHONY: clean, install

.SUFFIXES:

all: main.o util.o directory.o dosift.o cDoSift.cpp cdomatchlib.o cDoMatch.cpp cDoAll.cpp doall.o domatch.o dogeometry.o cdomatchlib.o
	$(CXX) main.o util.o dosift.o domatch.o dogeometry.o directory.o doall.o -gdwarf-2 --std=c++11 -o ulavalSFM -I../include/ -L../lib/ $(LIBS)
	mpic++ cDoSift.cpp util.o directory.o dosift.o -gdwarf-2 --std=c++11 -o cDoSift -I../include/ $(LIBS) -L../lib/ -lopencv_core -lopencv_highgui -lopencv_features2d -lopencv_nonfree -lopencv_calib3d -lopencv_flann
	mpic++ cDoMatch.cpp util.o directory.o dosift.o domatch.o dogeometry.o cdomatchlib.o -gdwarf-2 --std=c++11 -o cDoMatch -I../include/ $(LIBS) -L../lib/ -lopencv_core -lopencv_highgui -lopencv_features2d -lopencv_nonfree -lopencv_calib3d -lopencv_flann
	mpic++ cDoAll.cpp directory.o util.o dosift.o domatch.o dogeometry.o cdomatchlib.o -gdwarf-2 --std=c++11 -o cDoAll -I../include/ $(LIBS) -L../lib/ -lopencv_core -lopencv_highgui -lopencv_features2d -lopencv_nonfree -lopencv_calib3d -lopencv_flann

main.o: main.cpp util.h dosift.h domatch.h
	$(CXX) -gdwarf-2 --std=c++11 -c main.cpp -o main.o -I../include/ $(OPENCV)

util.o: util.cpp util.h directory.h
	$(CXX) -gdwarf-2 --std=c++11 -c util.cpp -o util.o -I../include/ $(OPENCV)

directory.o: directory.cpp directory.h util.h
	$(CXX) -gdwarf-2 --std=c++11 -c directory.cpp -o directory.o -I../include/ $(OPENCV)

dosift.o: dosift.cpp dosift.h directory.h util.h
	$(CXX) -gdwarf-2 --std=c++11 -c dosift.cpp -o dosift.o -I../include/ $(OPENCV)

domatch.o: domatch.cpp domatch.h dosift.h directory.h dogeometry.h util.h
	$(CXX) -gdwarf-2 --std=c++11 -c domatch.cpp -o domatch.o -I../include/ $(OPENCV)

dogeometry.o: dogeometry.cpp dogeometry.h domatch.h directory.h util.h
	$(CXX) -gdwarf-2 --std=c++11 -c dogeometry.cpp -o dogeometry.o -I../include/ $(OPENCV)

cdomatchlib.o: cDoMatchLib.cpp cDoMatchLib.h directory.h util.h dosift.h domatch.h dogeometry.h
	$(CXX) -gdwarf-2 --std=c++11 -c cDoMatchLib.cpp -o cdomatchlib.o -I../include/ $(OPENCV)

doall.o: doall.cpp doall.h util.h domatch.h dosift.h directory.h
	$(CXX) -gdwarf-2 --std=c++11 -c doall.cpp -o doall.o -I../include/ $(OPENCV)

clean:
	rm -rf *.o

install:
	cp ulavalSFM ../bin/
	cp ulavalSFM /usr/local/bin/
	cp cDoSift ../bin/
	cp cDoSift /usr/local/bin/
	cp cDoMatch ../bin/
	cp cDoMatch /usr/local/bin/
	cp cDoAll ../bin/
	cp cDoAll /usr/local/bin/

cinstall:
	cp ulavalSFM ../bin/
	cp cDoSift ../bin/
	cp cDoMatch ../bin
	cp cDoAll ../bin/
