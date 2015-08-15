/*
* md5-pass.c
*/

#include <stdio.h> // printf

#if defined(__APPLE__)
#	define COMMON_DIGEST_FOR_OPENSSL
#	include <CommonCrypto/CommonDigest.h>
#	define SHA1 CC_SHA1
#else
#	include <openssl/md5.h>
#endif

#define BUFFER_SIZE 512

int main (int argc, char *argv[]) {

	FILE * infile = stdin;

	MD5_CTX c;
	unsigned char digest[16];
	char out[33];
	MD5_Init(&c);

	char buffer[BUFFER_SIZE];
	size_t count = 0;
	while ((count = fread(buffer, 1, BUFFER_SIZE, infile)) > 0) {
		fwrite(buffer, 1, count, stdout);
		MD5_Update(&c, buffer, count);
	}

	MD5_Final(digest, &c);
	for (int n = 0; n < 16; ++n) {
		snprintf(&(out[n*2]), 16*2, "%02x", (unsigned int)digest[n]);
	}
	fprintf(stderr, "%s  %s\n", out, "-");

	return 0;
}
