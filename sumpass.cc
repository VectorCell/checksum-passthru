/*
* sumpass.cc
*/

#include <iostream>
#include <functional>

#include <cstdio>
#include <cstring>

#include "sumpass.h"

using namespace std;

typedef function<string(FILE*, FILE*)> digest_fn;

int main (int argc, char *argv[]) {

	digest_fn df;
	string algorithm;

	if (argc > 1) {
		algorithm = argv[1];
	} else {
		cout << "trying to determine algorithm programmatically" << endl;
		if (strstr(argv[0], "sumpass") != NULL) {
			cout << "called as sumpass" << endl;
			algorithm = "md5";
		}
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
