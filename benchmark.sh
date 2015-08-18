#!/bin/bash

TESTSIZE="$1"
if [ -z "$TESTSIZE" ]; then
	TESTSIZE="128M"
fi

if [ -n "$(which pv)" ]; then
	export PATH=.:$PATH

	HASHES="md5 sha1 sha224 sha256 sha384 sha512"

	TEMPFILE="sparse-$RANDOM.file"
	dd if=/dev/zero of=$TEMPFILE bs=1 count=0 seek=$TESTSIZE 2> /dev/null

	for HASH in $(echo $HASHES | tr ' ' '\n'); do
		printf "\n$HASH\n"
		echo "${HASH}sum"
		for i in {0..3}; do
			pv -cN "	" < $TEMPFILE | ${HASH}sum &> /dev/null
		done
		echo "${HASH}sump"
		for i in {0..3}; do
			pv -cN "	" < $TEMPFILE | ${HASH}sump &> /dev/null
		done
		echo "openssl $HASH"
		for i in {0..3}; do
			pv -cN "	" < $TEMPFILE | openssl $HASH &> /dev/null
		done
		echo "sump $HASH"
		for i in {0..3}; do
			pv -cN "	" < $TEMPFILE | sump $HASH &> /dev/null
		done
	done

	rm -f $TEMPFILE
else
	echo "pv not found, not benchmarking"
fi
