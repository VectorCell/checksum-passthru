#!/bin/bash

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
dir=$(dirname $(readlink -f $DIR/$(basename "$0")))

if [ "$(whoami)" == root ]; then
	bindir="/usr/local/bin"
else
	bindir="$HOME/bin"
fi
cd $bindir

#find $dir -maxdepth 1 -perm -111 -type f | grep -v "\.sh$" | while read file; do
#	src="$file"
#	dest="$bindir/$(basename $file)"
#	echo cp $src $dest
#	cp $src $dest
#done

ln -s $DIR/sump
