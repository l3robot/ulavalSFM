#!/usr/bin/python
### micro-program to check if there is png files among the jpg

import os
 
a = input("Enter the directory name here : ");
 
b = os.listdir(a)
 
for i in b:
  c = a + i
	f = open(c, "rb")
	image = f.read()
	if image[0] == 0x89 and image[1] == 0x50:
		print("Will transform " + i + " in PNG, because it's a PNG file...")
		name = i[0:-4] + ".png"
		command = "mv " + a + i + " " + a + name
		os.system(command)
