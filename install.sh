#!/bin/bash

# File : install.sh
# Author : Émile Robitaille @ LERobot
# Creation date : 07/02/2014
# Version : 1.0
# Description : Intall script if you don't have OpenCV. Linux version

echo "#Added by ulavalSFM" >> $HOME/.bashrc
echo "export LD_LIBRARY_PATH="$PWD/lib/:$LD_LIBRARY_PATH"" >> $HOME/.bashrc
echo "[ Adding library path in .bashrc file ... done ]"

cd lib/
git clone https://github.com/Itseez/opencv
echo "[ Cloning OpenCV ... done ]"

cd opencv/
mkdir build/
cd build/
cmake -D CMAKE_INSTALL_PREFIX="../../../" -D CMAKE_LIBRARY_PATH="../../" -D CMAKE_INCLUDE_PATH="../../../include/" ..
make -j 2
make install
echo "[ Building OpenCV ... done ]"

cd ../../../src/
make -j 2
make install
echo "[ Building ulavalSFM ... done ]" 

echo "[ YOU CAN RUN ULAVALSFM ]"
