/*
* sha1pass.c
*/

#include <stdio.h>
#include <openssl/sha.h>

#define BUFFER_SIZE (1024U * 32U)

int main (int argc, char *argv[]) {

	FILE * infile = stdin;

	SHA_CTX c;
	unsigned char digest[SHA_DIGEST_LENGTH];
	char out[SHA_DIGEST_LENGTH * 2 + 1];
	SHA1_Init(&c);

	unsigned char buffer[BUFFER_SIZE];
	size_t count = 0;
	while ((count = fread(buffer, 1, BUFFER_SIZE, infile)) > 0) {
		fwrite(buffer, 1, count, stdout);
		SHA1_Update(&c, buffer, count);
	}

	SHA1_Final(digest, &c);
	for (int n = 0; n < SHA_DIGEST_LENGTH; ++n) {
		snprintf(&(out[n*2]), SHA_DIGEST_LENGTH*2, "%02x", (unsigned int)digest[n]);
	}
	fprintf(stderr, "%s  %s\n", out, "-");

	return 0;
}
