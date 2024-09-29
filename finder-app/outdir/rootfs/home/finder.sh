#!/bin/sh

if [ -z $1 ];
then
    echo "Parameter 1 Not Passed"
    exit 1 
else
    filesdir=$1
fi

if [ -z $2 ];
then
    echo "Parameter 2 Not Passed"
    exit 1
else
    searchstr=$2
fi

# Testing for directory
if test -d $1
then
    echo "Directory Exist"
else
    echo "$1 does not represent a directory"    
    exit 1
fi

nFiles=$(grep -rl $1 -e $2 | wc -l)

nMatch=$(grep -roh $1 -e $2 | wc -l)

echo "The number of files are $nFiles and the number of matching lines are $nMatch"
