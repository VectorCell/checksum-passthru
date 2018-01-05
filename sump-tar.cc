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
#include <cctype>

#include "sump.h"

using namespace std;

vector<pair<string,string>>
tar_digest (FILE *infile,
            FILE *outfile,
            Digest& digest) {

	vector<pair<string,string>> output;
	char buf[RECORD_SIZE];
	TARFileHeader * const header = reinterpret_cast<TARFileHeader*>(&buf[0]);

	assert(sizeof(TARFileHeader) == RECORD_SIZE);
	char zeroblock[RECORD_SIZE];
	memset(zeroblock, 0, RECORD_SIZE);

	string filename;
	bool long_filename = false;
	size_t size = 0;
	char type = '\0';
	size_t count = 0;
	size_t record_num = 0;
	while (true) {

		count = tar_record_read(buf, infile, outfile);
		if (count < RECORD_SIZE) {
			break;
		}

		if (header->checkChecksum()) {

			// metadata
			if (long_filename) {
				long_filename = false;
			} else {
				filename = header->getFilename();
			}
			size = header->getFileSize();
			type = header->typeFlag;

			if (type == '\0' || type == '0') {
				// regular file

				// reads file
				digest.reset();
				while (size > 0) {
					size_t bytes_read = min(size, RECORD_SIZE);
					count = tar_record_read(buf, infile, outfile);
					size -= bytes_read;
					digest.update(buf, bytes_read);
				}
				output.push_back(make_pair(filename, digest.finalize()));

			} else if (type == '5') {
				// directory, do nothing

			} else if (type == 'L') {
				long_filename = true;
				filename = "";
				while (size > 0) {
					count = tar_record_read(buf, infile, outfile);
					filename += string(buf, min(size, RECORD_SIZE));
					size -= min(size, RECORD_SIZE);
				}
				filename.pop_back();

			} else {
				// unhandled type
				cerr << "ERROR: unhandled header type: " << type << endl;
			}
		} else {
			// doesn't match checksum, ignore for now
		}

		++record_num;
	}
	return output;
}

int main (int argc, char *argv[]) {
	string alg;
	for (int k = 1; k < argc; ++k) {
		alg = argv[k];
	}

	Digest digest = build_digest(alg);
	try {
		vector<pair<string,string>> sums = tar_digest(stdin, stdout, digest);
		sort(begin(sums), end(sums), filename_digest_comparator);
		for (const auto& p : sums) {
			cerr << p.second << "  " << p.first << "\n";
		}
		return 0;
	} catch (malformed_tar_error mte) {
		cerr << "ERROR: " << mte.what() << endl;
		return 1;
	}
}
