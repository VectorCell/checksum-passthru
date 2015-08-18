/*
* sha384sump.c
*/

#include <stdio.h>
#include <openssl/sha.h>

#define BUFFER_SIZE (1024U * 32U)

int main (int argc, char *argv[]) {

	FILE * infile = stdin;

	SHA512_CTX c;
	unsigned char digest[SHA384_DIGEST_LENGTH];
	char out[SHA384_DIGEST_LENGTH * 2 + 1];
	SHA384_Init(&c);

	unsigned char buffer[BUFFER_SIZE];
	size_t count = 0;
	while ((count = fread(buffer, 1, BUFFER_SIZE, infile)) > 0) {
		fwrite(buffer, 1, count, stdout);
		SHA384_Update(&c, buffer, count);
	}

	SHA384_Final(digest, &c);
	for (int n = 0; n < SHA384_DIGEST_LENGTH; ++n) {
		snprintf(&(out[n*2]), SHA384_DIGEST_LENGTH*2, "%02x", (unsigned int)digest[n]);
	}
	fprintf(stderr, "%s  %s\n", out, "-");

	return 0;
}
