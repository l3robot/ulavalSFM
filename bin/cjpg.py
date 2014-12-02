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
	end = i[len(i)-4:len(i)]
	ext = imghdr.what(i)
	if end == ".jpg" or end == ".JPG":
		if ext != "jpeg":
			c = a + i
			f = open(c, "rb")
			image = f.read()
			print(">> Will transform " + i + " in its true format, because it's not a jpeg file...")
			name = i[0:-4] + "." + ext
			command = "mv " + a + i + " " + a + name
			os.system(command)
