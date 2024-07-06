#!/bin/sh

if [ -z $1 ];
then
    echo "Parameter 1 Not Passed"
    exit 1 
else
    writefile=$1
fi

if [ -z $2 ];
then
    echo "Parameter 2 Not Passed"
    exit 1
else
    writestr=$2
fi

if test -d $writefile
then 
    echo "$writestr" > $writefile
else
    mkdir -p $(dirname "$writefile")
    echo "$writestr" > $writefile
fi

if [ "$?" -ne "0" ]
then
    echo "File Could not be Created"
    exit 1
fi
