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
            Digest& digest,
            bool debug = false,
            ostream& err = cerr) {

	if (debug) {
		err << "beginning tar_digest" << endl;
	}

	TARFileReader reader(infile, outfile);
	const char *buf;

	vector<pair<string,string>> output;

	string filename;
	bool long_filename = false;
	size_t size = 0;
	char type = '\0';
	size_t record_num = 0;
	while (true) {

		if (debug) {
			//err << endl << "RECORD " << record_num << endl;
		}

		buf = reader.fetch_record();
		const TARFileHeader *header = reinterpret_cast<const TARFileHeader *>(buf);
		
		if (buf == nullptr) {
			if (debug) {
				err << "reached end of input" << endl;
			}
			break;
		} else if (header->checkChecksum()) {

			// metadata
			if (long_filename) {
				long_filename = false;
			} else {
				filename = header->getFilename();
			}
			size = header->getFileSize();
			type = header->typeFlag;

			if (debug) {
				err << endl;
				err << "HEADER FOUND: type " << type << endl;
				err << "filename: " << filename << endl;
				err << "size:     " << size << endl;
			}

			if (type == '\0' || type == '0') {
				// regular file

				size_t n_records = (size / RECORD_SIZE) + (size % RECORD_SIZE == 0 ? 0 : 1);
				if (debug) {
					err << "total record count for file data: " << n_records << endl;
				}

				// reads file
				digest.reset();
				while (size > 0) {
					size_t bytes_read = min(size, RECORD_SIZE);
					// count = tar_record_read(buf, infile, outfile);
					buf = reader.fetch_record();
					size -= bytes_read;
					if (debug) {
						--n_records;
						if (!(n_records & 0xffffff)) {
							err << "records left: " << n_records << "\n";
						}
					}
					digest.update(buf, bytes_read);
				}
				if (debug) {
					err << "done reading file contents" << endl;
				}
				output.push_back(make_pair(filename, digest.finalize()));

			} else if (type == '5') {
				// directory, do nothing
				
			} else if (type == 'L') {
				long_filename = true;
				filename = "";
				while (size > 0) {
					// count = tar_record_read(buf, infile, outfile);
					buf = reader.fetch_record();
					filename += string(buf, min(size, RECORD_SIZE));
					size -= min(size, RECORD_SIZE);
				}
				filename.pop_back();

			} else {
				// unhandled type
				if (debug) {
					err << "ERROR: unhandled header type: " << type << endl;
					exit(1);
				}
			}
		} else {
			// doesn't match checksum, ignore for now
			if (debug) {
				//err << "DOESN'T MATCH CHECKSUM" << endl;
			}
		}

		++record_num;
	}
	return output;
}

int run_diagnostic_mode (int argc, char *argv[]) {
	string alg;
	for (int k = 1; k < argc; ++k) {
		alg = argv[k];
	}
	Digest digest = build_digest(alg);
	try {
		vector<pair<string,string>> sums = tar_digest(stdin, nullptr, digest, true);
		sort(begin(sums), end(sums), filename_digest_comparator);
		for (const auto& p : sums) {
			cout << p.second << "  " << p.first << endl;
		}
		return 0;
	} catch (malformed_tar_error mte) {
		cerr << "ERROR: " << mte.what() << endl;
		return 1;
	}
}

int run_normal_mode (int argc, char *argv[]) {
	string alg;
	for (int k = 1; k < argc; ++k) {
		alg = argv[k];
	}
	Digest digest = build_digest(alg);
	try {
		FILE *infile = stdin;
		FILE *outfile = stdout;
		if (isatty(fileno(outfile))) {
			outfile = nullptr;
		}
		vector<pair<string,string>> sums = tar_digest(infile, outfile, digest);
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

int main (int argc, char *argv[]) {
	return run_normal_mode(argc, argv);
	// return run_diagnostic_mode(argc, argv);
}
