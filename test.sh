#!/bin/bash

VALGRIND=$(which valgrind)
HASHES="md5 sha1 sha224 sha256 sha384 sha512"

test0 () {
	echo "hello world" | $VALGRIND ./${1}pass | tee >(${1}sum) >(openssl $1 | awk '{print $2}') > /dev/null
}

test1 () {
	dd if=/dev/urandom bs=1k count=1k 2> /dev/null | $VALGRIND ./${1}pass | tee >(${1}sum) >(openssl $1 | awk '{print $2}') > /dev/null
}

test2 () {
	$VALGRIND ./${1}pass < /usr/share/dict/words | tee >(${1}sum) >(openssl $1 | awk '{print $2}') > /dev/null
}

for HASH in $(echo $HASHES | tr ' ' '\n'); do
	printf "\n$HASH\n"
	for i in {0..2}; do
		echo "test$i"
		test$i $HASH | cat
	done
done
