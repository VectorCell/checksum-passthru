#!/bin/bash

export PATH=.:$PATH

HASHES="md5 sha1 sha224 sha256 sha384 sha512"

TEMPFILE="sparse-$RANDOM.file"
dd if=/dev/zero of=$TEMPFILE bs=1k count=0 seek=1M 2> /dev/null

for HASH in $(echo $HASHES | tr ' ' '\n'); do
	printf "\n$HASH\n"
	for POSTFIX in $(echo "sum pass" | tr ' ' '\n'); do
		pv -cN "$HASH$POSTFIX" < $TEMPFILE | $HASH$POSTFIX &> /dev/null
	done
done

rm -f $TEMPFILE
