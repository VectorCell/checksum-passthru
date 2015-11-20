#include "sump-thread.h"

#include <iostream>
#include <cstring>
#include <vector>

using namespace std;

int no_Init (void *) {

	cout << "this is no_Init" << endl;
	return 0;
}
int no_Update (void *, const void *, size_t) {
	return 0;
}
int no_Final (uint8_t *bytes, void *) {
	*bytes = 0;
	return 0;
}

int bytecount_Init (void *ptr) {
	cout << "this is bytecount_Init" << endl;
	*(uint32_t *)ptr = 0;
	return 0;
}
int bytecount_Update (void *ptr, const void *buf, size_t cnt) {
	*(uint32_t *)ptr += cnt;
	return 0;
}
int bytecount_Final (uint8_t *bytes, void *ptr) {
	void *ret = memcpy(bytes, ptr, sizeof(uint32_t));
	return (ret == bytes) ? 0 : 1;
}

int main (int argc, char *argv[]) {
	vector<digest_type> digests;

	void *null = NULL;
	digests.push_back(digest_type("none", 1, null, no_Init, no_Update, no_Final));

	uint32_t bytecount_ctx;
	digests.push_back(digest_type("bytecount",
			                      sizeof(uint32_t),
			                      (void *)&bytecount_ctx,
			                      bytecount_Init,
			                      bytecount_Update,
			                      bytecount_Final));

	calc_digests(digests, stdin, stdout);

	return 0;
}
