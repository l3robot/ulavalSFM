ulavalSFM installation
----------------------

Version : 1.0

Author : Émile Robitaille @ LERobot

Last update : 07/02/2014

DEPENDENCIES
------------

* cmake
* g++
* OpenMPI
* OpenCV 2.4.9


INSTALL INSTRUCTIONS
--------------------

DO :

```Bash
chmod +x install.sh
./install.sh
```

This script will :

- Add new path to your local LD\_LIBRARY\_PATH
- Build OpenCV 2.4.9 and install it in the ulavalSFM directory
- Build ulavalSFM and install it in ulavalSFM/bin/ directory

It can take a while. You can add more worker cores by changing the argument after _make -j_
