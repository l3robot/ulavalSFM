ulavalSFM installation
----------------------

Version : 1.0

Author : Émile Robitaille @ LERobot

Last update : 07/02/2014

DEPENDENCIES
------------

#### To build OpenCV

* cmake 2.8.10.2 : http://www.cmake.org
* git 1.8.2.1 http://git-scm.com (Test version, it should work with others. It is used only to clone)

#### To build ulavalSFM

* g++ 4.8.0 (it should work with 4.9.0) : https://gcc.gnu.org
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

- Add the new library path to your local LD\_LIBRARY\_PATH
- Add an export command of the new path in your .bashrc
- Clone OpenCV with git in \<ulavalSFM_path\>/lib/ directory
- Build OpenCV 2.4.9 and install it in the ulavalSFM directory
- Build ulavalSFM and install it in ulavalSFM/bin/ directory

It could take a while. You can add more worker cores by changing the argument after _make -j_ in the script.

#### If you already have OpenCV on your computer:

DO :

```Bash
cd <ulavalSFM_path>/src/
make
make install
```

Those instructions will build ulavalSFM and install it in ulavalSFM/bin/ directory.



