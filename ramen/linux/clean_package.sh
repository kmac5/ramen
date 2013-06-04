#!/bin/bash

if [ -z "$1" ]; then 
	echo usage: $0 package_dir
	exit
fi

# bin
chrpath -d $1/bin/ramen.bin
strip $1/bin/ramen.bin

# remove pyc and pyo files from python.
find $1/python/app -name *.pyc | xargs rm -fr
find $1/python/app -name *.pyo | xargs rm -fr
find $1/python/lib/python2.7 -name *.pyc | xargs rm -fr
find $1/python/lib/python2.7 -name *.pyo | xargs rm -fr

