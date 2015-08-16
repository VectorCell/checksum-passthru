#!/bin/bash

TESTSIZE="$1"
if [ -z "$TESTSIZE" ]; then
	TESTSIZE="1M"
fi

if [ -n "$(which pv)" ]; then
	export PATH=.:$PATH

	HASHES="md5 sha1 sha224 sha256 sha384 sha512"

	TEMPFILE="sparse-$RANDOM.file"
	dd if=/dev/zero of=$TEMPFILE bs=1 count=0 seek=$TESTSIZE 2> /dev/null

	for HASH in $(echo $HASHES | tr ' ' '\n'); do
		printf "\n$HASH\n"
		for POSTFIX in $(echo "sum pass" | tr ' ' '\n'); do
			pv -cN "$HASH$POSTFIX" < $TEMPFILE | $HASH$POSTFIX &> /dev/null
		done
		for POSTFIX in $(echo "pass sum" | tr ' ' '\n'); do
			pv -cN "$HASH$POSTFIX" < $TEMPFILE | $HASH$POSTFIX &> /dev/null
		done
	done

	rm -f $TEMPFILE
fi
