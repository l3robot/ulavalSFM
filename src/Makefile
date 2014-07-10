
# File : src/Makefile
# Author : Émile Robitaille @ LERobot
# Last update : 07/02/2014
# Version : 1.0
# Description : Makefile used to compile ulavalSFM.

.PHONY: clean, install

.SUFFIXES:

all: main.o util.o directory.o dosift.o cDoSift.cpp domatch.o 
	g++ main.o util.o dosift.o domatch.o directory.o -gdwarf-2 --std=c++11 -o ulavalSFM -I../include/ -L../lib/ -lopencv_core -lopencv_highgui -lopencv_features2d -lopencv_nonfree -lopencv_calib3d -lopencv_flann
	mpic++ cDoSift.cpp util.o directory.o dosift.o -gdwarf-2 --std=c++11 -o cDoSift -I../include/ -L../lib/ -lopencv_core -lopencv_highgui -lopencv_features2d -lopencv_nonfree -lopencv_calib3d -lopencv_flann
	mpic++ cDoMatch.cpp util.o directory.o dosift.o domatch.o -gdwarf-2 --std=c++11 -o cDoMatch -I../include/ -L../lib/ -lopencv_core -lopencv_highgui -lopencv_features2d -lopencv_nonfree -lopencv_calib3d -lopencv_flann

main.o: main.cpp util.h dosift.h domatch.h
	g++ -gdwarf-2 --std=c++11 -c main.cpp -o main.o -I../include/

util.o: util.cpp util.h directory.h
	g++ -gdwarf-2 --std=c++11 -c util.cpp -o util.o

directory.o: directory.cpp directory.h util.h
	g++ -gdwarf-2 --std=c++11 -c directory.cpp -o directory.o -I../include/

dosift.o: dosift.cpp dosift.h directory.h
	g++ -gdwarf-2 --std=c++11 -c dosift.cpp -o dosift.o -I../include/

domatch.o: domatch.cpp domatch.h directory.h
	g++ -gdwarf-2 --std=c++11 -c domatch.cpp -o domatch.o -I../include/

clean:
	rm -rf *.o

install:
	cp ulavalSFM ../bin/
	cp cDoSift ../bin/
	cp cDoMatch ../bin/
