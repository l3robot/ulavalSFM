#!/bin/bash

# File : install.sh
# Initial author : Émile Robitaille @ LERobot
# Major contribution : Yannick Hold @ Soravux
# Description : Intall script if you don't have OpenCV. Linux version

# Automatically abort on error
set -e

# Get the directory which holds the current file
DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
OUT="$DIR/out.txt"
N=`nproc`

# Parse arguments and set defaults
if [ $# -ne 1 ]; then
	arg1="-nocolosse"
else
	arg1=$1
fi

echo $arg1

# Append exports to .bashrc file
if ! grep -q ulavalSFM "$HOME/.bashrc"; then
    echo "# added by ulavalSFM 4.0 installer" >> $HOME/.bashrc
    echo "export LD_LIBRARY_PATH="$DIR/lib/:\$LD_LIBRARY_PATH"" >> $HOME/.bashrc
    echo "export PATH="$DIR/bin/:\$PATH"" >> $HOME/.bashrc
    echo "export PKG_CONFIG_PATH="$DIR/lib/pkgconfig:$PKG_CONFIG_PATH"" >> $HOME/.bashrc
    source $HOME/.bashrc
    echo "[ Adding library path in .bashrc file ... done ]"
fi

# Clone required dependencies
cd dependencies/

if [ $arg1 != "-colosse" ] && [ ! -d opencv ]; then
    git clone --single-branch https://github.com/Itseez/opencv.git opencv
    git clone https://github.com/Itseez/opencv_contrib.git
fi
if [ ! -d bundler_sfm ]; then
    if [ $arg1 != "-colosse" ]; then
        git clone https://github.com/LERobot/bundler_sfm
    else
        git clone https://github.com/lvsn/bundler_sfm
    fi
fi

# Build the dependencies

if [ $arg1 != "-colosse" ]; then
    cd opencv/
    if [ ! -d build ]; then
        mkdir build/
        cd build/
        cmake -DCMAKE_INSTALL_PREFIX="${DIR}" -DCMAKE_LIBRARY_PATH="${DIR}" -DCMAKE_INCLUDE_PATH="${DIR}/include/" -DOPENCV_EXTRA_MODULES_PATH=${DIR}/dependencies/opencv_contrib/modules -DBUILD_opencv_legacy=OFF ..
        make -j $N
        make install
    fi
fi


if [ $arg1 == "-colosse" ]; then
	cd bundler_sfm/
else
	cd ${DIR}/dependencies/bundler_sfm/
fi

if [ ! -e bin/bundler ]; then
    make -j $N
    rm -f bin/bundler.py
    mv bin/* ${DIR}/bin/
    mv lib/* ${DIR}/lib/
fi

cd ${DIR}/src/
make -j $N
make cinstall

chmod +x ${DIR}/bin/bundler.py
chmod +x ${DIR}/bin/cext.py
chmod +x ${DIR}/bin/cleanSFM.py

cd ${DIR}

echo "[ YOU CAN RUN ULAVALSFM ]"
