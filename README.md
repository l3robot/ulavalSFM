ulavalSFM
=========

Author : Émile Robitaille @ <a href=https://github.com/L3Robot>LERobot</a>
Major contribution : Yannick Hold @ <a href=https://github.com/soravux>soravux</a>

What is ulavalSFM ?
-------------------

ulavalSFM is a set of linux scripts and softwares to prepare structure from motion in parallel. It consists of three parts, pre-computation on images, Lowe's sift points search and matching of those points. You'll find explanations below to install dependencies, ulavalSFM and how to run it. A ultra-script will helps you install all the dependencies. Another will helps you run all softwares in one command. The extern softwares used to make a complete reconstruction are <a href=https://github.com/snavely/bundler_sfm>BundlerSFM</a>, <a href=http://www.di.ens.fr/cmvs/>CMVS</a>, <a href=http://www.di.ens.fr/pmvs/>PMVS2</a>.

Dependencies
------------

Here's what you have to install before running anything. All the specified versions are tested ones, it should work with others. If no version are given, use the last release :

* git 1.8.2.1 http://git-scm.com
* g++ 4.8.0 : https://gcc.gnu.org or

### Used to build openCV libraries

* cmake 2.8.10.2 : http://www.cmake.org

### Used to build BundlerSFM, CMVS, PMVS2

This <a href=http://adinutzyc21.blogspot.ca/2013/02/installing-bundler-on-linux-tutorial.html>link</a> helps to find all this dependencies

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
* <a href=http://www.netlib.org/clapack/>download</a> f2c.h and clapack.h, put it in a folder named clapack

### Used to build ulavalSFM softwares

* OpenMPI 1.6.5 : http://www.open-mpi.org
* OpenCV 3.0 : https://github.com/Itseez/opencv

### Used by scripts

* python interpreter 3.4.3
* PIL 1.1.7. Pillow for python 3.4 : http://www.pythonware.com/products/pil/

Because it's pretty simple to install and reliable, I strongly recommend anaconda python 3.4 package http://continuum.io/downloads#34 (make sure it is python 3.4). Be sure that your python bin path is $HOME/anaconda3/bin/ and it should work on any clusters.

Questions ? / Comments ?
------------------------

Don't hesitate, send me an email
emile.courriel@gmail.com
