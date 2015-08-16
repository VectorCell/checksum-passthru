/*
* sumpass.cc
*/

#include <iostream>
#include <functional>

#include <cstdio>
#include <cstring>

#include "sumpass.h"

using namespace std;

const string md5_digest (FILE* infile, FILE* outfile) {
	return openssl_digest(infile, outfile, MD5_DIGEST_LENGTH, MD5_Init, MD5_Update, MD5_Final);
}

const string sha1_digest (FILE* infile, FILE* outfile) {
	return openssl_digest(infile, outfile, SHA_DIGEST_LENGTH, SHA1_Init, SHA1_Update, SHA1_Final);
}

const string sha224_digest (FILE* infile, FILE* outfile) {
	return openssl_digest(infile, outfile, SHA224_DIGEST_LENGTH, SHA224_Init, SHA224_Update, SHA224_Final);
}

const string sha256_digest (FILE* infile, FILE* outfile) {
	return openssl_digest(infile, outfile, SHA256_DIGEST_LENGTH, SHA256_Init, SHA256_Update, SHA256_Final);
}

const string sha384_digest (FILE* infile, FILE* outfile) {
	return openssl_digest(infile, outfile, SHA384_DIGEST_LENGTH, SHA384_Init, SHA384_Update, SHA384_Final);
}

const string sha512_digest (FILE* infile, FILE* outfile) {
	return openssl_digest(infile, outfile, SHA512_DIGEST_LENGTH, SHA512_Init, SHA512_Update, SHA512_Final);
}

typedef function<string(FILE*, FILE*)> digest_fn;

int main (int argc, char *argv[]) {

	digest_fn df;
	string algorithm;

	if (argc > 1) {
		algorithm = argv[1];
	} else {
		cout << cin.rdbuf();
		cerr << "ERROR: algorithm not defined" << endl;
		return 1;
	}

	if (algorithm == "md5") {
		df = md5_digest;
	} else if (algorithm == "sha1") {
		df = sha1_digest;
	} else if (algorithm == "sha224") {
		df = sha224_digest;
	} else if (algorithm == "sha256") {
		df = sha256_digest;
	} else if (algorithm == "sha384") {
		df = sha384_digest;
	} else if (algorithm == "sha512") {
		df = sha512_digest;
	} else {
		cout << cin.rdbuf();
		cerr << "unkndown algorithm: " << algorithm << endl;
		return 1;
	}
	
	cerr << df(stdin, stdout) << endl;

	return 0;
}
