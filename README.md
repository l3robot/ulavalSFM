ulavalSFM
=========

###### Author : Émile Robitaille @ <a href=https://github.com/L3Robot>LERobot</a>
###### Major contribution : Yannick Hold @ <a href=https://github.com/soravux>soravux</a>

What is ulavalSFM ?
-------------------

ulavalSFM is a set of linux scripts and softwares to prepare structure from motion in parallel. It consists of three parts, pre-computation on images, Lowe's sift points search and matching of those points. You'll find explanations below to install dependencies, ulavalSFM and how to run it. A ultra-script will helps you install all the dependencies. Another will helps you run all softwares in one command. The extern softwares used to make a complete reconstruction are <a href=https://github.com/snavely/bundler_sfm>BundlerSFM</a>, <a href=http://www.di.ens.fr/cmvs/>CMVS</a>, <a href=http://www.di.ens.fr/pmvs/>PMVS2</a>.

If you want to read about Structure From Motion, here's a list of great papers :

* http://grail.cs.washington.edu/rome/rome_paper.pdf
* http://phototour.cs.washington.edu/Photo_Tourism.pdf
* http://phototour.cs.washington.edu/ModelingTheWorld_ijcv07.pdf

Dependencies
------------

Here's what you have to install before running anything. All the specified versions are tested ones, it should work with others. If no version are given, use the last release :

* git 1.8.2.1 http://git-scm.com
* g++ 4.8.0 : https://gcc.gnu.org

#### Used to build openCV libraries

* cmake 2.8.10.2 : http://www.cmake.org

#### Used to build BundlerSFM, CMVS, PMVS2

This <a href=http://adinutzyc21.blogspot.ca/2013/02/installing-bundler-on-linux-tutorial.html>link</a> helps me to find all this dependencies

* lapack 3.2.1 : http://www.netlib.org/lapack/
* blas : http://www.netlib.org/blas/
* cblas : http://www.netlib.org/blas/
* minpack : http://www.netlib.org/minpack/
* f2c : http://www.netlib.org/f2c/
* libjpeg : http://libjpeg.sourceforge.net/
* libzip : http://www.nih.at/libzip/
* imagemagik : http://www.imagemagick.org/
* gfortran : https://gcc.gnu.org/wiki/GFortran
* ceres-solver 1.9.0 : http://ceres-solver.org
* download Graclus <a href=http://www.cs.utexas.edu/users/dml/Software/graclus.html>here</a> version 1.2, put it in a folder named graclus
* download f2c.h and clapack.h <a href=http://www.netlib.org/clapack/>here</a>, put it in a folder named clapack

#### Used to build ulavalSFM softwares

* OpenMPI 1.6.5 : http://www.open-mpi.org
* OpenCV 3.0 : https://github.com/Itseez/opencv

#### Used by scripts

* Python 3.4.3 : https://www.python.org/
* Pillow 2.4.0 : http://www.pythonware.com/products/pil/

Because it's pretty simple to install and reliable, I strongly recommend anaconda python 3.4 package http://continuum.io/downloads#34 (make sure it is python 3.4). Be sure that your python bin path is $HOME/anaconda3/bin/ and it should work on any clusters.

Installation
------------

```Bash
cd "wherever_you_want_:)"
git clone https://github.com/lvsn/ulavalSFM
cd <ulavalSFM>/
bash install.sh
```

#### This script will :

- Adds the new library path to your local LD\_LIBRARY\_PATH
- Adds the new executable paths to your local PATH
- Adds their associated export commands in your home .bashrc
- Clones BundlerSFM~~, CMVS and PMVS2 to the dependencies/ repository~~
- Builds BundlerSFM~~, CMVS and PMVS2~~
- Moves all their associated libraries in the lib/ repository
- Moves all their associated binaries to the bin/ repository
- Builds ulavalSFM and install it in bin/ repository

Run it
------

#### ~~The easy way~~

```Bash
cd "in_your_dataset_repo"
ulavalSFM.py
```

#### The hard way

Use all the softs separately. It is more flexible though. You can change some options.

Pre-Computation
---------------

#### Usage

Lowe's sift points search
-------------------------

#### Usage

**What's printed on the screen :**

```Bash
This is ulsift (ulavalSFM sift). Use it to find sift points on a dataset.
Louis-Émile Robitaille @ L3Robot
usage: mpirun -n [numberOfCores] ulsift [-v] [-o Path] [workingDirectory]
      -v verbose mode, print a progress bar (default false)
      -o [siftPath] set the sift files repository (default ulsift/)
```

**More explanations :**

* -v toggle the verbose mode, so it will basically print information about the work distribution and a progress bar.
* -o You can choose another destination than ulsift/ for your sift points files

Matching phase
--------------

#### Usage

**What's printed on the screen :**

```Bash
This is ulmatch (ulavalSFM match). Use it to match the sift points you found.
Louis-Émile Robitaille @ L3Robot
usage: mpirun -n [numberOfCores] ulmatch [-vg] [-s Path] [-o Path] [-f Path] [workingDirectory]
      -v verbose mode, print a progress bar (default false)
      -g geometry mode, do some geometric computations (default false)
      -s [siftPath] set the sift directory path (default ulsift/)
      -o [matchFilePath] set the match file path (default matches.init.txt)
      -f [geoFilePath] set the geometric file path (default ulavalSFM.txt)
```

**More explanations :**

* -v toggle the verbose mode, so it will basically print information about the work distribution and a progress bar
* -s you can indicate another sift path if you have written sift point files elsewhere
* -o You can choose another destination and name than matches.init.txt for your match information

Geometry mode is not yet perfect, but prunes more bad matches and compute certain geometric information needed by the structure from motion.

Questions ? / Comments ?
------------------------

Don't hesitate, send me an email
emile.courriel@gmail.com
