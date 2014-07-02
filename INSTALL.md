ulavalSFM installation
----------------------

Version : 1.0

Author : Émile Robitaille @ LERobot

Last update : 07/02/2014

DEPENDENCIES
------------

#### To build OpenCV

* cmake 2.8.10.2 : http://www.cmake.org
* git 1.8.2.1 http://git-scm.com (Test version, should work with others. It is used only to clone)

#### To build ulavalSFM

* g++ 4.8.0 (should work with 4.9.0) : https://gcc.gnu.org
* OpenMPI 1.6.4 : http://www.open-mpi.org
* OpenCV 2.4.9 : https://github.com/Itseez/opencv

INSTALL INSTRUCTIONS
--------------------

#### If you don't have OpenCV, here is a way to install it with my script :

DO :

```Bash
chmod +x install.sh
./install.sh
```

This script will :

- Add new path to your local LD\_LIBRARY\_PATH
- Build OpenCV 2.4.9 and install it in the ulavalSFM directory
- Build ulavalSFM and install it in ulavalSFM/bin/ directory

It could take a while. You can add more worker cores by changing the argument after _make -j_

#### If you already have OpenCV on your computer:

DO :

```Bash
cd <ulavalSFM_path>/src/
make
make install
```

Those instructions will build ulavalSFM and install it in ulavalSFM/bin/ directory



