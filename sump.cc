/*
* sump.cc
*/

#include <iostream>
#include <functional>

#include <cstdio>
#include <unistd.h>
#include <cstring>

#include "sump.h"

using namespace std;

typedef function<string(FILE*, FILE*)> digest_fn;

digest_fn get_digest_fn(const string& alg) {
	digest_fn df = md5_digest;
	if (alg == "md5") {
		df = md5_digest;
	} else if (alg == "sha1") {
		df = sha1_digest;
	} else if (alg == "sha224") {
		df = sha224_digest;
	} else if (alg == "sha256") {
		df = sha256_digest;
	} else if (alg == "sha384") {
		df = sha384_digest;
	} else if (alg == "sha512") {
		df = sha512_digest;
	} else if (alg == "none") {
		df = no_digest;
	}
	return df;
}

int main (int argc, char *argv[]) {
	digest_fn df;
	string alg = "md5";
	FILE* infile;
	FILE* outfile;
	FILE* sumfile;
	string infile_name = "-";
	string outfile_name = "-";
	string sumfile_name = "-";
	bool is_infile = false;
	bool is_outfile = false;
	bool is_sumfile = false;
	for (int k = 0; k < argc; ++k) {
		if (is_infile) {
			is_infile = false;
			infile_name = argv[k];
		} else if (is_outfile) {
			is_outfile = false;
			outfile_name = argv[k];
		} else if (is_sumfile) {
			is_sumfile = false;
			sumfile_name = argv[k];
		} else {
			if (!strcmp("-i", argv[k])) {
				is_infile = true;
			} else if (!strcmp("-o", argv[k])) {
				is_outfile = true;
			} else if (!strcmp("-s", argv[k])) {
				is_sumfile = true;
			} else {
				alg = argv[k];
			}
		}
	}

	if (infile_name == "-") {
		infile = stdin;
	} else {
		infile = fopen(infile_name.c_str(), "r");
	}
	if (argc >= 3) {
		infile_name = argv[2];
	}

	if (outfile_name == "-") {
		if (isatty(fileno(stdout))) {
			outfile = NULL;
		} else {
			outfile = stdout;
		}
	} else {
		outfile = fopen(outfile_name.c_str(), "w");
	}

	if (sumfile_name == "-") {
		sumfile = stderr;
	} else {
		sumfile = fopen(sumfile_name.c_str(), "w");
	}

	df = get_digest_fn(alg);
	string digest = df(infile, outfile);
	fprintf(sumfile, "%s  %s\n", digest.c_str(), infile_name.c_str());
	return 0;
}
