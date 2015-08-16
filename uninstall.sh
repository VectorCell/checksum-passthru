#!/bin/bash

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
dir=$(dirname $(readlink -f $DIR/$(basename "$0")))

if [ "$(whoami)" == root ]; then
	bindir="/usr/local/bin"
else
	bindir="$HOME/bin"
fi
cd $bindir

for file in $(ls $dir | grep pass | grep -v -E "\.|Makefile"); do
	installed="$bindir/$file"
	if [ -e $installed ]; then
		echo rm $installed
		rm $installed
	fi
done
