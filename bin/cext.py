#!/usr/bin/env python
#USAGE# 
# cext.py

import os
from os.path import join, splitext
import sys

import imghdr



def renameExtMishaps(path):
    """Renames every misnamed image extension in {path}.
    Will also put in lowercase the extensions."""
    for filename in os.listdir(path):
        if not os.path.isfile(filename):
            continue
        basename, end = splitext(filename)
        end = end[1:]
        ext = imghdr.what(filename)
        
        try:
            ext = ext.lower()
        except AttributeError:
            print("Could not find the type of file {}.".format(filename))
            continue

        if ext == "jpeg":
            ext = "jpg"

        if end != ext:
            print("File {} will have its extension renamed to {}.".format(filename, ext))
            os.rename(join(path, filename), join(path, "{}.{}".format(basename, ext)))


if __name__ == '__main__':
    if len(sys.argv) < 2:
        path = input("Enter the directory name here : ")
    else:
        path = sys.argv[-1]

    renameExtMishaps(path)
