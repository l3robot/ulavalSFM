#!/usr/bin/env python

import argparse
import glob
import shutil
import os
import subprocess
from PIL import Image, ExifTags

VERSION="ulSFMtest 1.0"

DESCRIPTION="""This is ulSFMtest (ulavalSFM test). Use it to test ulsift and ulmatch.
Louis-Émile Robitaille @ L3Robot
"""

# class algoAction(argparse.Action):
#
#     def __call__(self, parser, namespace, values, option_string=None):
#         if values < 0 or values > 2:
#             parser.error("The choice of algo must be between 0 and 2")
#         setattr(namespace, self.dest, values)
#
# class resAction(argparse.Action):
#
#     def __call__(self, parser, namespace, values, option_string=None):
#         if values < 0:
#             parser.error("The resolution in percent must be positive")
#         setattr(namespace, self.dest, values)


###################################################################

def importImages(dataset, directory):

    for i in glob.iglob(os.path.join(dataset,"*.jpg")):
        des = os.path.split(i)
        des = os.path.join(directory, des[1])
        shutil.copy(i, des)

def eraseImages(directory):

    for i in glob.iglob(os.path.join(directory, "*.jpg")):
        os.remove(i)

    for i in glob.iglob(os.path.join(directory, "*.key")):
        os.remove(i)

def resizeImages(factor, directory):

    size = (0,0)

    for i in glob.iglob(os.path.join(directory, "*.jpg")):
        image = Image.open(i)
        size = image.size
        size = (int(size[0]*factor), int(size[1]*factor))
        new = image.resize(size)
        image.close()
        new.save(i)
        new.close()

    return size

def timeSift(directory):

    time = 0

    with open(os.path.join(directory, "ulsift.out"), 'r') as f:
        time = f.readline()
        time = f.readline()
        time = time[26:]

    return time

def averageSift(verbose, directory):

    keys = 0
    number = 0

    for i in glob.iglob(os.path.join(directory, "*.key")):
        with open(i, 'r') as f:
            header = f.readline()
            header = header.split()
            keys = keys + int(header[0])
            image = os.path.split(i)
            image = image[1]
            # if verbose:
            #     print("      k- {0:} as {1:10d} sift points".format(image, int(header[0])))
        number = number + 1

    return keys/number

def siftTests(verbose, maximize, maxCores, maxRes, minRes, step, dataset, directory):

    env = dict(os.environ)

    garbage = open(os.path.join(directory, "garbage"), "w")

    if verbose:
        print(" -- Beginning sift tests:")

    begin = 1;

    if maximize:
        begin = maxCores

    for i in range(begin,maxCores+1):
        if verbose:
            print("    > Using {0} cores:".format(i))
        for j in range(minRes, maxRes+step, step):
            eraseImages(directory)
            importImages(dataset, directory)
            size = resizeImages(j/100, directory)
            if verbose:
                print("    > Using {0} resolution:".format(size))
            subprocess.call(["mpirun", "-n", str(i), "ulsift", "-o",
            directory, directory], shell=False,
            env=env, stdout=garbage)
            time = timeSift(directory)
            average = averageSift(verbose, directory)
            if verbose:
                print("      i- {0:.3f} sift poits on average".format(average))
                print("      i- {0} to find sift points".format(time[:len(time)-1]))


def startTests(verbose, maximize, maxCores, maxRes, minRes, step, algo, dataset):

    directory = os.path.join(dataset, "ulSFMtest/")

    if verbose :
        print(" -- Starting tests:")
        print("    > {0} core(s) max".format(maxCores))
        print("    > {0}% to {1}% for resolution".format(minRes, maxRes))
        if maximize:
            print("    > It will only use maximum core value")

    if not os.path.exists(directory):
        os.makedirs(directory)

    if (algo != 1):
        siftTests(verbose, maximize, maxCores, maxRes, minRes, step, dataset, directory)

    #TO DO: match

    if verbose :
        print(" -- End of tests:")

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=DESCRIPTION)
    parser.add_argument('-v', '--verbose', action='store_true',
        help="generate additional output in execution", default=False)
    parser.add_argument('-m', '--maximize-core', action='store_true',
        help="Just use the max core value", default=False)
    parser.add_argument('--version', action='version', version=VERSION)
    parser.add_argument('--cores-max', type=int,
        help="Number of cores max for test", default=1)
    parser.add_argument('--reso-max', type=int,
        help="Max resolution for images in %", default=200)
    parser.add_argument('--reso-min', type=int,
        help="Min resoltuion for images in %", default=100)
    parser.add_argument('--reso-step', type=int,
        help="Step between min and max resolution in %", default=20)
    parser.add_argument('--algo', type=int,
        help="0 : sift | 1 : match | 2 : both", default=0)
    parser.add_argument('--dataset', type=str,
        help="Image dataset path", default=".")
    args = parser.parse_args()

    startTests(
        verbose=args.verbose,
        maximize=args.maximize_core,
        maxCores=args.cores_max,
        maxRes=args.reso_max,
        minRes=args.reso_min,
        step=args.reso_step,
        algo=args.algo,
        dataset=args.dataset
    )
