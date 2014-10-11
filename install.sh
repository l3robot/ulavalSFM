#!/bin/bash

# File : install.sh
# Author : Émile Robitaille @ LERobot
# Last update : 07/03/2014
# Version : 1.0
# Description : Intall script if you don't have OpenCV. Linux version

N="1"
OUT="$PWD/out.txt"

#echo "#Added by ulavalSFM" >> $HOME/.bashrc
#echo "export LD_LIBRARY_PATH="$PWD/lib/:$LD_LIBRARY_PATH"" >> $HOME/.bashrc
export LD_LIBRARY_PATH="$PWD/lib/:$LD_LIBRARY_PATH"
echo "[ Adding library path in .bashrc file ... done ]"

cd lib/
#only 2.4.9 branch
git clone -b 2.4.9.x-prep --single-branch https://github.com/Itseez/opencv.git opencv-2.4.9 >> $OUT 2>&1
if [ $? -ne 0 ]; then
	echo "[ ERROR : cloning OpenCV, check out.txt for more information ]"
	return
else
	echo "[ Cloning OpenCV ... done ]"
fi
#For colosse cluster users
if [ $1 == "-colosse" ]; then
	git clone https://github.com/lvsn/bundler_sfm >> $OUT 2>&1
else
	git clone https://github.com/LERobot/bundler_sfm >> $OUT 2>&1
fi
if [ $? -ne 0 ]; then
	echo "[ ERROR : Cloning BundlerSFM, check out.txt for more information ]"
	return
else
	echo "[ Cloning BundlerSFM ... done ]"
fi

cd opencv-2.4.9/
if [ -d build ]; then
	echo "[ WARNING : build directory already exists]"
else
	mkdir build/
	echo "[ Creation of build directory ... done]"
fi
cd build/
cmake -D CMAKE_INSTALL_PREFIX="../../../" -D CMAKE_LIBRARY_PATH="../../" -D CMAKE_INCLUDE_PATH="../../../include/" .. >> out.txt 2>&1
make -j $N >> $OUT 2>&1
if [ $? -ne 0 ]; then
	echo "[ ERROR : Building OpenCV, check out.txt for more information ]"
	return
else
	echo "[ Building OpenCV ... done ]"
fi
make install >> $OUT 2>&1
if [ $? -ne 0 ]; then
	echo "[ ERROR : Installing OpenCV, check out.txt for more information ]"
	return
else
	echo "[ Installing OpenCV ... done ]"
fi

cd ../../bundler_sfm/
make clean >> $OUT 2>&1
make -j 2 >> $OUT 2>&1
if [ $? -ne 0 ]; then
	echo "[ ERROR : Building BundlerSFM, check out.txt for more information ]"
	return
else
	echo "[ Building BundlerSFM ... done ]"
fi
rm -f bin/bundler.py
mv bin/* ../../bin/ >> $OUT 2>&1
mv lib/* ../../ >> $OUT 2>&1
if [ $? -ne 0 ]; then
	echo "[ ERROR : Installing BundlerSFM, check out.txt for more information ]"
	return
else
	echo "[ Installing BundlerSFM ... done ]"
fi

cd ../../src/
make clean >> $OUT 2>&1
make -j 2 $OUT >> 2>&1
if [ $? -ne 0 ]; then
	echo "[ ERROR : Building ulavalSFM, check out.txt for more information ]"
	return
else
	echo "[ Building ulavalSFM ... done ]"
fi
make install $OUT >> 2>&1
if [ $? -ne 0 ]; then
	echo "[ ERROR : Installing ulavalSFM, check out.txt for more information ]"
	return
else
	echo "[ Installing ulavalSFM ... done ]"
fi

echo "[ YOU CAN RUN ULAVALSFM ]"
