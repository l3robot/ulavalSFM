ulavalSFM
=========

###### Author : Émile Robitaille @ <a href=https://github.com/L3Robot>LERobot</a>
###### Major contribution : Yannick Hold @ <a href=https://github.com/soravux>soravux</a>

What is ulavalSFM ?
-------------------

ulavalSFM is a set of linux scripts and softwares to prepare structure from motion in parallel. It consists of three parts, pre-computation on images, Lowe's sift points search and matching of those points. You'll find explanations below to install dependencies, ulavalSFM and how to run it. A ultra-script will helps you install all the dependencies. Another will helps you run all softwares in one command. The extern softwares used to make a complete reconstruction are <a href=https://github.com/snavely/bundler_sfm>BundlerSFM</a>, <a href=http://www.di.ens.fr/cmvs/>CMVS</a>, <a href=http://www.di.ens.fr/pmvs/>PMVS2</a>.

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
- Clones BundlerSFM, CMVS and PMVS2 to the dependencies/ repository
- Builds BundlerSFM, CMVS and PMVS2
- Moves all their associated libraries in the lib/ repository
- Moves all their associated binaries to the bin/ repository
- Builds ulavalSFM and install it in bin/ repository

Run it
------

#### The easy way

```Bash
cd "in_your_dataset_repo"
ulavalSFM.py
```

#### The hard way

Use all the softs separately. It is more flexible though. You can change some options.

Pre-Computation
---------------

Complete the computation when it's done.

Lowe's sift points search
-------------------------

#### What's a sift point?

On a given image, a Scale-invariant feature transform (sift) is a point of interest with characteristics that will be mostly identical in another image that consists of the geometric transformation of the first.

You can learn more on Lowe's sift points on this <a href=http://www.scholarpedia.org/article/Scale_Invariant_Feature_Transform>website</a> and on its wikipedia <a href=http://fr.wikipedia.org/wiki/Scale-invariant_feature_transform>page</a>. Here's the <a href=http://www.cs.ubc.ca/~lowe/keypoints/>website</a> of the Lowe's keypoints detector.

#### How do I detect the sift points in my code?

With openCV 3.0, it's pretty easy to detect sift point on an image. In fact, it takes me three lines of code. I create the detector with this <a href=google.com>line</a> :

```c
Ptr<SIFT> ptrSIFT = SIFT::create();
```

and I detect those points thanks to those <a href=google.com>lines</a> :

```c
ptrSIFT->detect(img, keypoints);

ptrSIFT->compute(img, keypoints, des);
```

#### What's parallel?

I had to isolate a part of the algorithm and distribute it to the worker cores. I choose to distribute the detection of sift point, because it's already an independent task. Each core begin to compute its starting index and ending index in preparation to execute a single loop, where an index represents an image to compute. Here's the algorithm I use to compute find the right index :

```c
/*
 id => id of the core
 size => number of cores
 dir => working directory infos
 start => starting index
 ending => ending index
*/

int numimages = dir.getNBImages();

int distFactor = numimages / size;
int distError = numimages % size;

if (id < distError)
	*start = id * (distFactor + 1);
	*end = (id + 1) * (distFactor + 1);

else
	*start = id * distFactor + distError;
	*end = *start + distFactor;

```

Each working core are used to write in the files, since there's a file output for each image describing its sift points.

#### What's the format of output files?

There's one file for each computed images. The format is the Lowe's format, that is:  

Header :

2 int</br>
[Number of points] [Descriptor size]\n

First point :</br>

4 float</br>
[y coordinate] [x coordinate] [scale] [angle (rad)]\n

128 int with this disposition</br>
[] [] [] [] [] [] [] [] [] [] [] [] [] [] [] [] [] [] [] []\n</br>
[] [] [] [] [] [] [] [] [] [] [] [] [] [] [] [] [] [] [] []\n</br>
[] [] [] [] [] [] [] [] [] [] [] [] [] [] [] [] [] [] [] []\n</br>
[] [] [] [] [] [] [] [] [] [] [] [] [] [] [] [] [] [] [] []\n</br>
[] [] [] [] [] [] [] [] [] [] [] [] [] [] [] [] [] [] [] []\n</br>
[] [] [] [] [] [] [] [] [] [] [] [] [] [] [] [] [] [] [] []\n</br>
[] [] [] [] [] [] [] [] [] [] [] [] [] [] [] [] [] [] [] []\n</br>
[] [] [] [] [] [] [] [] [] [] [] [] [] [] []\n

Second point : ...

Note that I use size instead of scale in my files, because openCV does not give scale.

#### Usage



Questions ? / Comments ?
------------------------

Don't hesitate, send me an email
emile.courriel@gmail.com
