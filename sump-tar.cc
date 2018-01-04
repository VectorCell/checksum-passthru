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


bool filename_digest_comparator (pair<string,string> a,
                                 pair<string,string> b) {
	return a.first < b.first;
}

// vector<pair<string,string>>
// tar_digest (FILE *infile,
//             FILE *outfile,
//             AbstractDigest* digest) {
// 	vector<pair<string,string>> output;
// 	size_t count = 0;
// 	char buf[RECORD_SIZE * RECORDS_PER_READ];
// 	bool next_is_header = true;
// 	bool take_sum = true;
// 	size_t bytes_remaining = 0;
// 	string filename;
// 	while ((count = fread(buf, 1, sizeof(buf), infile)) > 0) {
// 		fwrite(buf, 1, count, outfile);
// 		for (size_t off = 0; off < sizeof(buf); off += RECORD_SIZE) {
// 			if (next_is_header) {
// 				tar_posix_header *h = reinterpret_cast<tar_posix_header*>(&buf[off]);
// 				if (h->name[0] == '\0')
// 					continue;
// 				take_sum = (h->typeflag == '\0' || h->typeflag == '0');
// 				string name = h->name;
// 				string prefix = h->prefix;
// 				filename = prefix + name;
// 				size_t size = decode(h->size, 8);
// 				bytes_remaining = size;
// 				if (bytes_remaining > 0) {
// 					next_is_header = false;
// 				}
// 				if (take_sum)
// 					digest->reset();
// 			} else {
// 				size_t n_bytes = min(bytes_remaining, RECORD_SIZE);
// 				if (take_sum)
// 					digest->update(&buf[off], n_bytes);
// 				if (bytes_remaining > RECORD_SIZE) {
// 					bytes_remaining -= RECORD_SIZE;
// 				} else {
// 					if (take_sum) {
// 						string hexdigest = digest->finalize();
// 						output.push_back(make_pair(filename, hexdigest));
// 					}
// 					next_is_header = true;
// 				}
// 			}
// 		}
// 	}
// 	sort(begin(output), end(output), filename_digest_comparator);
// 	return output;
// }

size_t read_tar_record (char *buf, FILE *f, size_t count = RECORD_SIZE) {
	size_t res = fread(buf, 1, count, f);
	// if (res != RECORD_SIZE) {
	// 	// throw malformed_tar_error("tar read not large enough");
	// 	throw malformed_tar_error("reached premature EOF");
	// }
	return res;
}

size_t write_tar_record (char *buf, FILE *out, size_t count = RECORD_SIZE) {
	size_t res = fwrite(buf, 1, count, out);
	return res;
}

vector<pair<string,string>>
tar_digest (FILE *infile,
            FILE *outfile,
            Digest& digest) {

	vector<pair<string,string>> output;
	// vector<char> buffer(RECORD_SIZE);
	// char *buf = &buffer[0];
	char buf[RECORD_SIZE];
	TARFileHeader * const header = reinterpret_cast<TARFileHeader*>(&buf[0]);


	assert(sizeof(TARFileHeader) == RECORD_SIZE);
	char zeroblock[RECORD_SIZE];
	memset(zeroblock, 0, RECORD_SIZE);

	bool next_entry_has_long_name = false;
	size_t count = 0;
	while (true) {

		count = read_tar_record(buf, infile);
		if (count < RECORD_SIZE) {
			break;
		}

		string filename(
			header->filename,
			min(static_cast<size_t>(100), strlen(header->filename)));
		size_t prefix_len = strlen(header->filenamePrefix);
		if (prefix_len > 0) {
			filename = string(header->filenamePrefix) + "/" + filename;
		}

		if (header->checkChecksum()) {
			// cerr << "THIS IS A FILE HEADER (record " << record_num << ")" << endl;
			// cerr << "filename: " << filename << endl;
			// cerr << "type:     " << header->typeFlag << endl;
			// cerr << "size:     " << header->getFileSize() << endl;

			// regular file
			if (header->typeFlag == '\0' || header->typeFlag == '0') {

				if (next_entry_has_long_name) {
					filename = string(header->filename);
					// the next header contains the metadata
					count = read_tar_record(buf, infile);
					if (count != RECORD_SIZE) {
						throw malformed_tar_error(
							"reached premature EOF while fetching extented header");
					}
					next_entry_has_long_name = false;
				}

				size_t size = header->getFileSize();
				size_t bytes_remaining = size;

				// reads file
				digest.reset();
				while (bytes_remaining > 0) {
					size_t bytes_read = min(bytes_remaining, RECORD_SIZE);
					count = read_tar_record(buf, infile);
					// if (count != RECORD_SIZE) {
					// 	throw malformed_tar_error(
					// 		"reached premature EOF while fetching file contents");
					// }
					count = write_tar_record(buf, outfile, count);
					bytes_remaining -= bytes_read;
					digest.update(buf, bytes_read);
				}
				output.push_back(make_pair(filename, digest.finalize()));

			} else if (header->typeFlag == '5') {
				// directory
			} else if (header->typeFlag == 'L') {
				next_entry_has_long_name = true;
			} else {
				// unhandled type
			}
		} else {
			// doesn't match checksum
		}
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
