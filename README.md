ulavalSFM
=========

Version : 1.0

Author : Émile Robitaille @ LERobot

Last update : 07/02/2014

What is ulavalSFM ?
-------------------

ulavalSFM is a free software manager to prepare and do structure from motion in a parallel way. It's in development. Now, only the sift part is implemented. The structure from motion will be based on bundlerSFM : https://github.com/soravux/bundler_sfm 

Usage
-----

* -h  ---      : Print this menu
* -v  ---      : Print the software version
* -l [dir]     : Print information about the directory
* -c [0-1]     : On cluster or not. If 1, a script .sh file will be generated (default 0)
* -n [1-*]     : Specify the number of core(s) wanted (default 1, means no mpi)
* -s [dir]     : To find sift features of the directory images
* -m [dir]     : To match sift features of the directory images
* -a [dir]     : Do "-s dir" and then "-m dir"

More details :


-l [dir] : Will give the directory name, number of images, .sift files and .mat files.

-c [0-1] : The software will use Torque msub to submit the .sh file. Not implemented yet. You will have the possibility to change the dispatcher in a configuration file.

-n [1-*] : It uses OpenMPI to launch the extern program cDoSift on multiple cores.

-s [dir] : Will do sift detection using OpenCV 2.4.9 implementation and write the features in a Lowe's binairy format.

-m [dir] : Will do match using OpenCV 2.4.9. It uses knn search to find the two best matches and uses a ratio test of 0.8 to eliminate most of bad maches. Not implemented yet.

Questions ? 
-----------

Send me an email
erobotaille@gmail.com









