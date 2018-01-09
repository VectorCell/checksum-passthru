#!/bin/bash

# export PATH=.:$PATH
# #VALGRIND="valgrind"
# HASHES="md5 sha1 sha224 sha256 sha384 sha512"

# accuracytest0 () {
# 	echo "hello world" | ${1}sum | awk '{print $1}'
# 	echo "hello world" | ${1}sump > /dev/null
# 	echo "hello world" | openssl ${1} | awk '{print $2}'
# 	echo "hello world" | sump ${1} > /dev/null
# }

# accuracytest1 () {
# 	TEMPFILE=/tmp/tempfile-$RANDOM.file
# 	dd if=/dev/urandom of=$TEMPFILE bs=1k count=1k 2> /dev/null
# 	${1}sum < $TEMPFILE | awk '{print $1}'
# 	${1}sump < $TEMPFILE > /dev/null
# 	openssl ${1} < $TEMPFILE | awk '{print $2}'
# 	sump ${1} < $TEMPFILE > /dev/null
# 	rm -f $TEMPFILE
# }

# accuracytest2 () {
# 	${1}sum < /usr/share/dict/words | awk '{print $1}'
# 	${1}sump < /usr/share/dict/words > /dev/null
# 	openssl ${1} < /usr/share/dict/words | awk '{print $2}'
# 	sump ${1} < /usr/share/dict/words > /dev/null
# }

# memorytest0 () {
# 	$VALGRIND ${1}sump < /usr/share/dict/words > /dev/null
# 	$VALGRIND sump $1 < /usr/share/dict/words > /dev/null
# }

# # not meant for benchmarking, only for identifying serious speed problems
# speedtest0 () {
# 	SPARSEFILE="sparse-$RANDOM.file"
# 	dd if=/dev/zero of=$SPARSEFILE bs=1k count=0 seek=8k 2> /dev/null
# 	for POSTFIX in $(echo "sum sump" | tr ' ' '\n'); do
# 		echo "$1$POSTFIX"
# 		time $1$POSTFIX < $SPARSEFILE &> /dev/null
# 	done
# 	for PROG in $(echo "openssl sump" | tr ' ' '\n'); do
# 		echo "$PROG $1"
# 		time $PROG $1 < $SPARSEFILE &> /dev/null
# 	done
# 	rm $SPARSEFILE
# }

# for HASH in $(echo $HASHES | tr ' ' '\n'); do
# 	printf "\n$HASH\n"
# 	for i in {0..2}; do
# 		echo "accuracytest$i"
# 		accuracytest$i $HASH 2>&1 | cat
# 	done | awk '{print $1}'
# 	if [ -n "$VALGRIND" ]; then
# 		for i in {0..0}; do
# 			echo "memorytest$i"
# 			memorytest$i $HASH
# 		done | awk '{print $1}'
# 	fi
# 	for i in {0..0}; do
# 		echo "speedtest$i"
# 		speedtest$i $HASH
# 	done
# done

if [ ! -f test.tar ]; then
	if [ ! -d test ]; then
		mkdir test
		dd if=/dev/urandom of=test/rand1.file bs=4k count=1
		dd if=/dev/urandom of=test/rand2.file bs=4k count=2
		dd if=/dev/urandom of=test/rand3.file bs=4k count=3
		dd if=/dev/urandom of=test/rand4.file bs=4k count=4
		echo "hello world" > test/text1.txt
		echo "this is the second file" > test/text2.txt
		dd if=/dev/zero of=test/zeroes1.file bs=4k count=0 seek=4k
		mkdir test/films
		dd if=/dev/zero of=test/films/fake.mkv bs=1 count=0 seek=258374273
		# dd if=/dev/zero of="test/films/Star.Wars.Episode.II.Attack.of.the.Clones.2002.Blu-ray.Remux.1080p.AVC.DTS-HD.MA.6.1-HDRemuX.mkv" bs=1 count=0 seek=36281333103
		# dd if=/dev/zero of="test/films/Star.Wars.Episode.III.Revenge.of.the.Sith.2005.Blu-ray.Remux.1080p.AVC.DTS-HD.MA.6.1-HDRemuX.mkv" bs=1 count=0 seek=36588542602
		# dd if=/dev/zero of="test/films/Star.Wars.Episode.I.The.Phantom.Menace.1999.Blu-ray.Remux.1080p.AVC.DTS-HD.MA.6.1-HDRemuX.mkv" bs=1 count=0 seek=36606908124
		# dd if=/dev/zero of="test/films/Star.Wars.Episode.IV.A.New.Hope.1977.Blu-ray.Remux.1080p.AVC.DTS-HD.MA.6.1-HDRemuX.mkv" bs=1 count=0 seek=35657974339
		# dd if=/dev/zero of="test/films/Star.Wars.Episode.VI.Return.of.the.Jedi.1983.Blu-ray.Remux.1080p.AVC.DTS-HD.MA.6.1-HDRemuX.mkv" bs=1 count=0 seek=36090294018
		# dd if=/dev/zero of="test/films/Star.Wars.Episode.V.The.Empire.Strikes.Back.1980.Blu-ray.Remux.1080p.AVC.DTS-HD.MA.6.1-HDRemuX.mkv" bs=1 count=0 seek=35885186582
		# dd if=/dev/zero of="test/films/Star.Wars.The.Complete.Saga.1977-2005.Blu-ray.Remux.1080p.AVC.DTS-HD.MA.6.1-HDRemuX.nfo" bs=1 count=0 seek=77918
	fi
	tar -c test | pv -Ws $(du -bd 0 test | awk '{print $1}') | output-sparse test.tar
	# tar -c test > test.tar
fi

# echo "sump md5 vs md5sum:"
# md5sum test.tar
# ./sump md5 < test.tar > /dev/null
# echo

# echo "output of sump-tar md5:"
# ./sump-tar md5 < test.tar > /dev/null
# echo

# echo "comparison of input and output of sump-tar md5:"
# ./sump md5 < test.tar | ./sump-tar md5 2> /dev/null | ./sump md5 > /dev/null
# echo

./sump-tar md5 < test.tar > /dev/null

# rm -f test.tar
