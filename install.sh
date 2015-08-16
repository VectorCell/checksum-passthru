#!/bin/bash

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
dir=$(dirname $(readlink -f $DIR/$(basename "$0")))

bindir="$HOME/bin"
cd $bindir

for file in $(ls $dir | grep pass | grep -v ".c"); do
	src="$dir/$file"
	dest="$bindir/$file"
	echo ln -s $src $dest
	ln -s $src $dest
done

