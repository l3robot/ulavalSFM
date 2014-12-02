#!/usr/bin/python
### micro-program to check if there is png files among the jpg

#USAGE# 
# cjpg.py

import os
 
a = input("Enter the directory name here : ")

if a[len(a)-1] != '/':
	a = a + '/'
 
b = os.listdir(a)
 
for i in b:
	ext = i[len(i)-3:len(i)-1]
	if ext == "jpg":
		c = a + i
		f = open(c, "rb")
		image = f.read()
		if image[0] == 0x89 and image[1] == 0x50:
			print("Will transform " + i + " in PNG, because it's a PNG file...")
			name = i[0:-4] + ".png"
			command = "mv " + a + i + " " + a + name
			os.system(command)
