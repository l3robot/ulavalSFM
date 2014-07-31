
# File : src/Makefile
# Author : Émile Robitaille @ LERobot
# Last update : 07/02/2014
# Version : 1.0
# Description : Makefile used to compile ulavalSFM.

.PHONY: clean, install

.SUFFIXES:

all: main.o util.o directory.o dosift.o cDoSift.cpp cDoMatch.cpp cDoMatch.h cDoAll.cpp doall.o domatch.o dogeometry.o
	g++ main.o util.o dosift.o domatch.o dogeometry.o directory.o doall.o -gdwarf-2 --std=c++11 -o ulavalSFM -I../include/ -L../lib/ -lopencv_core -lopencv_highgui -lopencv_features2d -lopencv_nonfree -lopencv_calib3d -lopencv_flann
	mpic++ cDoSift.cpp util.o directory.o dosift.o -gdwarf-2 --std=c++11 -o cDoSift -I../include/ -L../lib/ -lopencv_core -lopencv_highgui -lopencv_features2d -lopencv_nonfree -lopencv_calib3d -lopencv_flann
	mpic++ cDoMatch.cpp util.o directory.o dosift.o domatch.o dogeometry.o -gdwarf-2 --std=c++11 -o cDoMatch -I../include/ -L../lib/ -lopencv_core -lopencv_highgui -lopencv_features2d -lopencv_nonfree -lopencv_calib3d -lopencv_flann
	mpic++ cDoAll.cpp directory.o util.o dosift.o domatch.o dogeometry.o cDoMatch.cpp -gdwarf-2 --std=c++11 -o cDoAll -I../include/ -L../lib/ -lopencv_core -lopencv_highgui -lopencv_features2d -lopencv_nonfree -lopencv_calib3d -lopencv_flann

main.o: main.cpp util.h dosift.h domatch.h
	g++ -gdwarf-2 --std=c++11 -c main.cpp -o main.o -I../include/

util.o: util.cpp util.h directory.h
	g++ -gdwarf-2 --std=c++11 -c util.cpp -o util.o -I../include/

directory.o: directory.cpp directory.h util.h
	g++ -gdwarf-2 --std=c++11 -c directory.cpp -o directory.o -I../include/

dosift.o: dosift.cpp dosift.h directory.h util.h
	g++ -gdwarf-2 --std=c++11 -c dosift.cpp -o dosift.o -I../include/

domatch.o: domatch.cpp domatch.h dosift.h directory.h dogeometry.h util.h
	g++ -gdwarf-2 --std=c++11 -c domatch.cpp -o domatch.o -I../include/

dogeometry.o: dogeometry.cpp dogeometry.h domatch.h directory.h
	g++ -gdwarf-2 --std=c++11 -c dogeometry.cpp -o dogeometry.o -I../include/

doall.o: doall.cpp doall.h util.h domatch.h dosift.h directory.h
	g++ -gdwarf-2 --std=c++11 -c doall.cpp -o doall.o -I../include/

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
	cp ulavalSFM $(SCRATCH)
	cp cDoSift ../bin/
	cp cDoSift $(SCRATCH)
	cp cDoMatch ../bin/
	cp cDoMatch $(SCRATCH)
	cp cDoAll ../bin/
	cp cDoAll $(SCRATCH)
