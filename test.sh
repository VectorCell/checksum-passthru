#!/bin/bash

export PATH=.:$PATH
#VALGRIND="valgrind"
HASHES="md5 sha1 sha224 sha256 sha384 sha512"

accuracytest0 () {
	echo "hello world" | ${1}sum | awk '{print $1}'
	echo "hello world" | ${1}pass > /dev/null
	echo "hello world" | openssl ${1} | awk '{print $2}'
	echo "hello world" | sumpass ${1} > /dev/null
}

accuracytest1 () {
	TEMPFILE=/tmp/tempfile-$RANDOM.file
	dd if=/dev/urandom of=$TEMPFILE bs=1k count=1k 2> /dev/null
	${1}sum < $TEMPFILE | awk '{print $1}'
	${1}pass < $TEMPFILE > /dev/null
	openssl ${1} < $TEMPFILE | awk '{print $2}'
	sumpass ${1} < $TEMPFILE > /dev/null
	rm -f $TEMPFILE
}

accuracytest2 () {
	${1}sum < /usr/share/dict/words | awk '{print $1}'
	${1}pass < /usr/share/dict/words > /dev/null
	openssl ${1} < /usr/share/dict/words | awk '{print $2}'
	sumpass ${1} < /usr/share/dict/words > /dev/null
}

memorytest0 () {
	$VALGRIND ${1}pass < /usr/share/dict/words > /dev/null
	$VALGRIND sumpass $1 < /usr/share/dict/words > /dev/null
}

speedtest0 () {
	SPARSEFILE="sparse-$RANDOM.file"
	dd if=/dev/zero of=$SPARSEFILE bs=1k count=0 seek=16k 2> /dev/null
	for POSTFIX in $(echo "sum pass" | tr ' ' '\n'); do
		echo "$1$POSTFIX"
		time $1$POSTFIX < $SPARSEFILE &> /dev/null
	done
	rm $SPARSEFILE
}

for HASH in $(echo $HASHES | tr ' ' '\n'); do
	printf "\n$HASH\n"
	for i in {0..2}; do
		echo "accuracytest$i"
		accuracytest$i $HASH 2>&1 | cat
	done | awk '{print $1}'
	if [ -n "$VALGRIND" ]; then
		for i in {0..0}; do
			echo "memorytest$i"
			memorytest$i $HASH
		done | awk '{print $1}'
	fi
	for i in {0..0}; do
		echo "speedtest$i"
		speedtest$i $HASH
	done
done
