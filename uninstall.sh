#!/bin/bash

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
dir=$(dirname $(readlink -f $DIR/$(basename "$0")))

bindir="$HOME/bin"
cd $bindir

for file in $(ls $dir | grep pass | grep -v ".c"); do
	installed="$bindir/$file"
	if [ -e $installed ]; then
		echo rm -f $installed
		rm -f $installed
	fi
done

