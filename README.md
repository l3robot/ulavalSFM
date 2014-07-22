ulavalSFM
=========

Version : 3.0

Author : Émile Robitaille @ LERobot

Last update : 07/22/2014

What is ulavalSFM ?
-------------------

ulavalSFM is a free software manager to prepare and do structure from motion in a parallel way. It's in development. Now, only the sift and the matching part are implemented. The structure from motion will be based on bundlerSFM : https://github.com/snavely/bundler_sfm. My version of BundlerSFM will though recognize the "ulavalSFM.txt" file and the new "matches.init.txt" file. For now, do all the 3 preparation parts on multiple cores with ulavalSFM, cd in the working directory and launch the modified "bundler.py" script available in my forked bundlerSFM repo.   

Usage
-----

#### Displayed on terminal

* -h  ---      : Print this menu
* -v  ---      : Print the software version
* -l [dir]     : Print information about the directory
* -c [0-1]     : On cluster or not. If 1, a script .sh file will be generated (default 0)
* -n [1-*]     : Specify the number of core(s) wanted (default 1, means no mpi)
* -s [dir]     : To find sift features of the directory images
* -m [dir]     : To match sift features of the directory images
* -g [dir]     : To compute geometric constraints on the directory images
* -a [dir]     : Do "-s dir", "-m dir" and then "-g dir"

#### More details :

-l [dir] : Will give the directory name, number of images, .key files and .mat files.

-c [0-1] : The software will use Torque msub to submit the .sh file. Not implemented yet. You will have the possibility to change the dispatcher in a configuration file.

-n [1-*] : It uses OpenMPI to launch the extern program cDoSift on multiple cores.

-s [dir] : Will do sift detection using OpenCV 2.4.9 implementation and write the features in a Lowe's binairy format.

-m [dir] : Will do match using OpenCV 2.4.9. It uses knn search to find the two best matches and uses a ratio test of 0.6 to eliminate most of bad maches.

-g [dir] : It will pruned the double matches, pruned the outliers with a fundamental matrix found using RANSAC and compute geometric constraints needed by bundlerSFM to begin the structure from motion with a homographic matrix found using RANSAC as well. I use OpenCV to compute the matrix and the inliers.

Notes
-----

#### Sift format

The four numbers before the descriptor in Lowe's sift file format are : 

| X coordinate | Y coordinate | scale | angle |

Note that OpenCV does not give scale so I used size instead to make the format compatible with program like Changchang Wu's visualSFM which natively use the Lowe's format. This will not influence my program because we do not use scale in structure from motion, but keep it in mind if you want to use my sifts for other purposes. 

Questions ? 
-----------

Send me an email
erobotaille@gmail.com









