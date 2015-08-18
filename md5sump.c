/*
* md5sump.c
*/

#include <stdio.h>
#include <openssl/md5.h>

#define BUFFER_SIZE (1024U * 32U)

int main (int argc, char *argv[]) {

	FILE * infile = stdin;

	MD5_CTX c;
	unsigned char digest[MD5_DIGEST_LENGTH];
	char out[33];
	MD5_Init(&c);

	unsigned char buffer[BUFFER_SIZE];
	size_t count = 0;
	while ((count = fread(buffer, 1, BUFFER_SIZE, infile)) > 0) {
		fwrite(buffer, 1, count, stdout);
		MD5_Update(&c, buffer, count);
	}

	MD5_Final(digest, &c);
	for (int n = 0; n < MD5_DIGEST_LENGTH; ++n) {
		snprintf(&(out[n*2]), MD5_DIGEST_LENGTH*2, "%02x", (unsigned int)digest[n]);
	}
	fprintf(stderr, "%s  %s\n", out, "-");

	return 0;
}
