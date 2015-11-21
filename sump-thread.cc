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

int bytecount_Init (void *ctx) {
	cout << "this is bytecount_Init" << endl;
	*(uint32_t *)ctx = 0;
	return 0;
}
int bytecount_Update (void *ctx, const void *buf, size_t cnt) {
	*(uint32_t *)ctx += cnt;
	return 0;
}
int bytecount_Final (uint8_t *bytes, void *ctx) {
	void *ret = memcpy(bytes, ctx, sizeof(uint32_t));
	return (ret == bytes) ? 0 : 1;
}

int main (int argc, char *argv[]) {

	vector<digest_type> digests;

	digests.push_back({"none", 1, NULL, no_Init, no_Update, no_Final, DIGEST_MAGIC});

	uint32_t bytecount_ctx;
	digests.push_back({"bytecount",
			           sizeof(uint32_t),
			           (void *)&bytecount_ctx,
			           bytecount_Init,
			           bytecount_Update,
			           bytecount_Final,
			           DIGEST_MAGIC});

	calc_digests(digests, stdin, stdout);

	return 0;
}
