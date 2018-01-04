#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <functional>
#include <algorithm>
#include <stdexcept>
#include <utility>

#include <cstdio>
#include <unistd.h>
#include <cstring>
#include <cassert>

#include "sump.h"

using namespace std;


size_t read_num_base (char *str, int radix) {
	size_t num = 0;
	while (*str != '\0') {
		num *= radix;
		num += *str - '0';
		++str;
	}
	return num;
}

bool filename_digest_comparator (pair<string,string> a,
                                 pair<string,string> b) {
	return a.first < b.first;
}

vector<pair<string,string>>
tar_digest (FILE *infile,
            FILE *outfile,
            AbstractDigest* digest) {
	vector<pair<string,string>> output;
	size_t count = 0;
	char buf[RECORD_SIZE * RECORDS_PER_READ];
	bool next_is_header = true;
	bool take_sum = true;
	size_t bytes_remaining = 0;
	string filename;
	while ((count = fread(buf, 1, sizeof(buf), infile)) > 0) {
		fwrite(buf, 1, count, outfile);
		for (size_t off = 0; off < sizeof(buf); off += RECORD_SIZE) {
			if (next_is_header) {
				tar_posix_header *h = reinterpret_cast<tar_posix_header*>(&buf[off]);
				if (h->name[0] == '\0')
					continue;
				take_sum = (h->typeflag == '\0' || h->typeflag == '0');
				string name = h->name;
				string prefix = h->prefix;
				filename = prefix + name;
				size_t size = read_num_base(h->size, 8);
				bytes_remaining = size;
				if (bytes_remaining > 0) {
					next_is_header = false;
				}
				if (take_sum)
					digest->reset();
			} else {
				size_t n_bytes = min(bytes_remaining, RECORD_SIZE);
				if (take_sum)
					digest->update(&buf[off], n_bytes);
				if (bytes_remaining > RECORD_SIZE) {
					bytes_remaining -= RECORD_SIZE;
				} else {
					if (take_sum) {
						string hexdigest = digest->finalize();
						output.push_back(make_pair(filename, hexdigest));
					}
					next_is_header = true;
				}
			}
		}
	}
	sort(begin(output), end(output), filename_digest_comparator);
	return output;
}

int main (int argc, char *argv[]) {
	string alg;
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
				alg = argv[k];
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

	vector<pair<string,string>> sums = tar_digest(infile,
	                                              outfile,
	                                              build_digest(alg));
	for (const auto& p : sums) {
		fprintf(sumfile, "%s  %s\n", p.second.c_str(), p.first.c_str());
	}
	return 0;
}
