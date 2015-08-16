#!/bin/bash

if [ -n "$(which xxhsum)" ]; then
	tee >(xxhsum 1>&2)
else
	1>&2 echo "xxhsum not found"
	cat
fi
