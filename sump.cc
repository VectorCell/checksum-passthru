/*
* sump.cc
*/

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <functional>
#include <algorithm>
#include <stdexcept>

#include <cstdio>
#include <unistd.h>
#include <cstring>
#include <cassert>

#include "sump.h"

using namespace std;

vector<string> multi_digest (FILE *infile,
                             FILE *outfile,
                             vector<Digest>& digests) {
	assert(infile != nullptr);
	if (digests.size() > 0) {
		unsigned char buffer[BUFFER_SIZE];
		size_t count = 0;
		while ((count = fread(buffer, 1, BUFFER_SIZE, infile)) > 0) {
			if (outfile != nullptr) {
				fwrite(buffer, 1, count, outfile);
			}
			if (digests.size() > 1) {
				#pragma omp parallel for
				for (unsigned k = 0; k < digests.size(); ++k) {
					digests[k].update(buffer, count);
				}
			} else {
				digests[0].update(buffer, count);
			}
		}
	}
	vector<string> outputs(digests.size());
	for (unsigned k = 0; k < digests.size(); ++k) {
		outputs[k] = digests[k].finalize();
	}
	return outputs;
}

int main (int argc, char *argv[]) {
	vector<string> algs;
	vector<Digest> digests;
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
			} else if (k > 0) {
				algs.push_back(argv[k]);
				digests.push_back(build_digest(argv[k]));
			}
		}
	}

	if (infile_name == "-") {
		infile = stdin;
	} else {
		infile = fopen(infile_name.c_str(), "r");
	}

	if (outfile_name == "-") {
		if (isatty(fileno(stdout))) {
			outfile = nullptr;
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

	if (digests.size() == 0) {
		digests.push_back(build_digest_default());
	}

	vector<string> sums = multi_digest(infile, outfile, digests);
	if (sums.size() > 1) {
		for (unsigned k = 0; k < algs.size(); ++k) {
			fprintf(sumfile, "%-8s  %s  %s\n",
				algs[k].c_str(),
				sums[k].c_str(),
				infile_name.c_str());
		}
	} else {
		fprintf(sumfile, "%s  %s\n", sums[0].c_str(), infile_name.c_str());
	}
	return 0;
}
