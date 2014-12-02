#!/bin/bash

#Script to quickly change extension of file 

#USAGE# 
# $1 = old extension
# $2 = new extension
# ext.sh <ext1> <ext2>

 
LIST="$(ls | grep $1)"; 
 
for i in $LIST; 
do mv $i ${i/$1/$2};
done
