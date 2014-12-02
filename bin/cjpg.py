#!/usr/bin/python
### micro-program to check if there is png files among the jpg

#USAGE# 
# cjpg.py

import os
import imghdr
 
a = input("Enter the directory name here : ")

if a[len(a)-1] != '/':
	a = a + '/'
 
b = os.listdir(a)
 
for i in b:
	ext = imghdr.what(i)
	if ext != "jpeg":
		c = a + i
		f = open(c, "rb")
		image = f.read()
		if image[0] == 0x89 and image[1] == 0x50:
			print(">> Will transform " + i + " in its true format, because it's not a jpeg file...")
			name = i[0:-4] + "." + ext
			command = "mv " + a + i + " " + a + name
			os.system(command)
