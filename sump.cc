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
                             const vector<AbstractDigest*>& digests) {
	assert(infile != nullptr);
	assert(outfile != nullptr);
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
					digests[k]->update(buffer, count);
				}
			} else {
				digests[0]->update(buffer, count);
			}
		}
	}
	vector<string> outputs(digests.size());
	for (unsigned k = 0; k < digests.size(); ++k) {
		outputs[k] = digests[k]->finalize();
	}
	return outputs;
}

int main (int argc, char *argv[]) {
	map<string,AbstractDigest*> digest_map;
	digest_map["none"] = build_digest_none();
	digest_map["md5"] = build_digest_md5();
	digest_map["sha1"] = build_digest_sha1();
	digest_map["sha224"] = build_digest_sha224();
	digest_map["sha256"] = build_digest_sha256();
	digest_map["sha384"] = build_digest_sha384();
	digest_map["sha512"] = build_digest_sha512();
	vector<string> algs;
	vector<AbstractDigest*> digests;
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
				string alg = argv[k];
				try {
					AbstractDigest* digest = digest_map.at(alg);
					auto pred = [digest] (AbstractDigest* other) -> bool {
						return digest == other;
					};
					if (!any_of(begin(digests), end(digests), pred)) {
						algs.push_back(alg);
						digests.push_back(digest);
					}
				} catch (out_of_range oor) {}
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
			outfile = NULL;
			fprintf(stderr, "ERROR: cannot output stream data to TTY.\n");
			return 1;
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

	vector<string> sums = multi_digest(infile, outfile, digests);
	for (unsigned k = 0; k < algs.size(); ++k) {
		fprintf(sumfile, "%s  [%s]  %s\n",
			sums[k].c_str(),
			algs[k].c_str(),
			infile_name.c_str());
	}
	for (auto const& p : digest_map) {
		delete p.second;
	}
	return 0;
}
