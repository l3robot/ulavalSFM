ulavalSFM installation
----------------------

Version : 3.0

Author : Émile Robitaille @ LERobot

Last update : 10/05/2014

DEPENDENCIES
------------

#### To build OpenCV

* cmake 2.8.10.2 : http://www.cmake.org
* git 1.8.2.1 http://git-scm.com (Test version, it should work with others. It is used only to clone)

#### To build bundler_sfm

* lapack 3.2.1 : http://www.netlib.org/lapack/
* ceres-solver 1.9.0 : http://ceres-solver.org

#### To build ulavalSFM

* g++ 4.8.0 (it should work with 4.9.0) : https://gcc.gnu.org
* OpenMPI 1.6.5 : http://www.open-mpi.org
* OpenCV 2.4.9 : https://github.com/Itseez/opencv
* bundler\_sfm (My modified version, based on modified soravux's version. <a href=https://github.com/snavely>Noah Snavely</a>'s <a href=https://github.com/snavely/bundler\_sfm>original</a>) : https://github.com/LERobot/bundler_sfm
 

#### To read the exif, used by the python script

* PIL 1.1.7. With Pillow for python 3.3 : http://www.pythonware.com/products/pil/

INSTALL INSTRUCTIONS
--------------------

#### If you don't have OpenCV and/or don't have the rights on the computer, here is a way to install it and install bundler_sfm with my linux script :

DO :

```Bash
git clone https://github.com/LERobot/ulavalSFM
cd <ulavalSFM>/
chmod +x install.sh
source install.sh
```

This script will :

- Add the new library path to your local LD\_LIBRARY\_PATH
- Add the new executable paths to your local PATH
- Add export commands of the new paths in your home .bashrc
- Clone OpenCV with git in \<ulavalSFM_path\>/lib/ directory
- Build OpenCV 2.4.9 and install it in the ulavalSFM directory
- Build bundler_sfm
- Build ulavalSFM and install it in \<ulavalSFM_path\>/bin/ directory

It could take a while. You can add more worker cores by changing the argument after _make -j_ in the script.

Note that is better to install it with a package management system, but if you are on a supercomputer and thus you don't have the permission, this script can save you time.

#### If you already have OpenCV and rights on your computer:

Please make sure openCV libraries are in your local lib folder

DO :

```Bash
git clone https://github.com/LERobot/ulavalSFM
cd <ulavalSFM_path>/lib/
git clone https://github.com/LERobot/bundler_sfm
cd bundler_sfm/
make clean
make
echo "export LD_LIBRARY_PATH=$PWD/lib/:$LD_LIBRARY_PATH" >> $HOME/.bashrc
export PATH="$PWD/lib/:$LD_LIBRARY_PATH"
echo "export PATH=$PWD/bin/:$PATH" >> $HOME/.bashrc
export PATH="$PWD/bin/:$PATH"
cd ../../src/
make
make install
```

Those instructions will build ulavalSFM and install it in \<ulavalSFM_path\>/bin/ directory.



