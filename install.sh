#!/bin/bash

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
dir=$(dirname $(readlink -f $DIR/$(basename "$0")))

if [ "$(whoami)" == root ]; then
	bindir="/usr/local/bin"
else
	bindir="$HOME/bin"
fi
cd $bindir

for file in $(ls $dir | grep pass | grep -v ".c"); do
	src="$dir/$file"
	dest="$bindir/$file"
	if [ "$(whoami)" == root ]; then
		echo cp $src $dest
		cp $src $dest
	else
		echo ln -s $src $dest
		ln -s $src $dest
	fi
done

