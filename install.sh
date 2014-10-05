#!/bin/bash

# File : install.sh
# Author : Émile Robitaille @ LERobot
# Last update : 07/03/2014
# Version : 1.0
# Description : Intall script if you don't have OpenCV. Linux version

N="2"

echo "#Added by ulavalSFM" >> $HOME/.bashrc
echo "export LD_LIBRARY_PATH="$PWD/lib/:$LD_LIBRARY_PATH"" >> $HOME/.bashrc
export LD_LIBRARY_PATH="$PWD/lib/:$LD_LIBRARY_PATH"
echo "[ Adding library path in .bashrc file ... done ]"

cd lib/
git clone https://github.com/Itseez/opencv >> out.txt 2>&1
if [ $? -eq 0 ]; then
	echo "[ ERROR : cloning OpenCV, check out.txt for more information ]"
else
	echo "[ Cloning OpenCV ... done ]"
fi
git clone https://github.com/LERobot/bundler_sfm >> out.txt 2>&1
if [ $? -eq 0 ]; then
	echo "[ ERROR : Cloning BundlerSFM, check out.txt for more information ]"
else
	echo "[ Cloning BundlerSFM ... done ]"
fi

cd opencv/
if [ -d build ]; then
	echo "[ build directory already exists]"
else
	mkdir build/
	echo "[ build directory creation ... done]"
fi
cd build/
cmake -D CMAKE_INSTALL_PREFIX="../../../" -D CMAKE_LIBRARY_PATH="../../" -D CMAKE_INCLUDE_PATH="../../../include/" .. >> out.txt 2>&1
make -j $(N) >> out.txt 2>&1
if [ $? -eq 0 ]; then
	echo "[ ERROR : Building OpenCV, check out.txt for more information ]"
else
	echo "[ Building OpenCV ... done ]"
fi
make install >> out.txt 2>&1
if [ $? -eq 0 ]; then
	echo "[ ERROR : Installing OpenCV, check out.txt for more information ]"
else
	echo "[ Installing OpenCV ... done ]"
fi

cd ../../bundler_sfm/
make clean >> out.txt 2>&1
make -j 2 >> out.txt 2>&1
if [ $? -eq 0 ]; then
	echo "[ ERROR : Building BundlerSFM, check out.txt for more information ]"
else
	echo "[ Building BundlerSFM ... done ]"
fi
mv bin/* ../../bin/ >> out.txt 2>&1
mv lib/* ../../lib/ >> out.txt 2>&1
if [ $? -eq 0 ]; then
	echo "[ ERROR : Installing BundlerSFM, check out.txt for more information ]"
else
	echo "[ Installing BundlerSFM ... done ]"
fi

cd ../../src/
make clean >> out.txt 2>&1
make -j 2 out.txt >> 2>&1
if [ $? -eq 0 ]; then
	echo "[ ERROR : Building ulavalSFM, check out.txt for more information ]"
else
	echo "[ Building ulavalSFM ... done ]"
fi
make install out.txt >> 2>&1
if [ $? -eq 0 ]; then
	echo "[ ERROR : Installing ulavalSFM, check out.txt for more information ]"
else
	echo "[ Installing ulavalSFM ... done ]"
fi

echo "[ YOU CAN RUN ULAVALSFM ]"
