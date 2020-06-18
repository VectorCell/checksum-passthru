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
#include <csignal>

#include "sump.h"


using namespace std;


void signal_handler (int sig) {
	if (sig == SIGPIPE) {
		// do nothing
	}
}

vector<pair<string,string>>
tar_digest (FILE *infile,
            FILE *outfile,
            Digest& digest,
            bool debug = false,
            ostream& err = cerr) {

	if (debug) {
		err << "beginning tar_digest" << endl;
	}

	if (digest.is_program_digest()) {
		signal(SIGPIPE, signal_handler);
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
			type = header->typeFlag;
			if (long_filename) {
				long_filename = false;
			} else {
				filename = header->getFilename();
			}
			if (debug) {
				err << endl;
				err << "HEADER FOUND: type " << type << endl;
				err << "filename: " << filename << endl;
			}
			size = header->getFileSize();
			if (debug) {
				err << "size:     " << size << endl;
			}

			if (type == '\0' || type == '0') {
				// regular file

				// reads file
				digest.reset();
				while (size > 0) {
					size_t bytes_read = min(size, RECORD_SIZE);
					buf = reader.fetch_record();
					size -= bytes_read;
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
		FILE *infile = stdin;
		FILE *outfile = stdout;
		if (isatty(fileno(outfile))) {
			outfile = nullptr;
		}
		vector<pair<string,string>> sums = tar_digest(infile, outfile, digest, true);
		sort(begin(sums), end(sums), filename_digest_comparator);
		for (const auto& p : sums) {
			cerr << digest.format(p.first, p.second) << "\n";
		}
		return 0;
	} catch (malformed_tar_error mte) {
		cerr << "ERROR: " << mte.what() << endl;
		return 1;
	}
}

int run_normal_mode (int argc, char *argv[]) {
	bool sums_only = false;
	string alg;
	for (int k = 1; k < argc; ++k) {
		string arg = argv[k];
		if (arg == "--sumsonly")
			sums_only = true;
		else
			alg = arg;
	}
	Digest digest = build_digest(alg);
	try {
		FILE *infile = stdin;
		FILE *outfile = stdout;
		if (sums_only || isatty(fileno(outfile))) {
			outfile = nullptr;
		}
		vector<pair<string,string>> sums = tar_digest(infile, outfile, digest);
		sort(begin(sums), end(sums), filename_digest_comparator);
		for (const auto& p : sums) {
			if (sums_only)
				cout << digest.format(p.first, p.second) << "\n";
			else
				cerr << digest.format(p.first, p.second) << "\n";
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
