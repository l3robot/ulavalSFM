#!/usr/bin/env python
### micro-program to clean the directory before running ulavalSFM

#USAGE#
# cleanSFM.py

import os
from os.path import join, splitext

from cext import renameExtMishaps


renameExtMishaps(".")
try:
    os.makedirs("backup")
except FileExistsError:
    pass

for idx, filename in enumerate(os.listdir(".")):
    if not os.path.isfile(filename):
        continue
    basename, ext = splitext(filename)
    ext = ext[1:]
    target_filename = "{}.jpg".format(idx)

    # Don't overwrite a file that would be named as the target
    if os.path.isfile(target_filename):
        ren_idx = 1
        rep_base, rep_ext = splitext(target_filename)
        replace_name = "{}_{}.{}".format(rep_base, ren_idx, rep_ext)
        while os.path.isfile(replace_name):
            ren_idx += 1
            replace_name = "{}_{}.{}".format(rep_base, ren_idx, rep_ext)
        os.rename(target_filename, replace_name)

    # TODO: Compute resize factor if needed
    # TODO: Various enhancements (contrast?)
    # Converting to jpeg
    if ext != "jpg":
        print(">> Converting file {} to jpeg...".format(filename))
        retval = os.system("convert {} {}".format(filename, target_filename))
        if retval != 0:
            print(">> Could not convert file {} to jpeg!".format(filename))
        os.rename(filename, join("backup", filename))
    else:
        os.rename(filename, target_filename)
