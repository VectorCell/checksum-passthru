#ifndef SUMPASS_H
#define SUMPASS_H

#include <cstdio>
#include <string>
#include <openssl/sha.h>

#ifndef BUFFER_SIZE
#	define BUFFER_SIZE (1024U * 32U)
#else
#	error "BUFFER_SIZE already defined"
#endif

const std::string md5_digest (FILE* infile, FILE* outfile) {
	MD5_CTX c;
	unsigned char digest[MD5_DIGEST_LENGTH];
	char out[MD5_DIGEST_LENGTH * 2 + 1];
	MD5_Init(&c);
	unsigned char buffer[BUFFER_SIZE];
	size_t count = 0;
	while ((count = fread(buffer, 1, BUFFER_SIZE, infile)) > 0) {
		fwrite(buffer, 1, count, outfile);
		MD5_Update(&c, buffer, count);
	}
	MD5_Final(digest, &c);
	for (int n = 0; n < MD5_DIGEST_LENGTH; ++n) {
		snprintf(&(out[n*2]), MD5_DIGEST_LENGTH*2, "%02x", (unsigned int)digest[n]);
	}
	return std::string(out);
}

const std::string sha1_digest (FILE* infile, FILE* outfile) {
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
	return std::string(out);
}

const std::string sha224_digest (FILE* infile, FILE* outfile) {
	SHA256_CTX c;
	unsigned char digest[SHA224_DIGEST_LENGTH];
	char out[SHA224_DIGEST_LENGTH * 2 + 1];
	SHA224_Init(&c);
	unsigned char buffer[BUFFER_SIZE];
	size_t count = 0;
	while ((count = fread(buffer, 1, BUFFER_SIZE, infile)) > 0) {
		fwrite(buffer, 1, count, stdout);
		SHA224_Update(&c, buffer, count);
	}
	SHA224_Final(digest, &c);
	for (int n = 0; n < SHA224_DIGEST_LENGTH; ++n) {
		snprintf(&(out[n*2]), SHA224_DIGEST_LENGTH*2, "%02x", (unsigned int)digest[n]);
	}
	return std::string(out);
}

const std::string sha256_digest (FILE* infile, FILE* outfile) {
	SHA256_CTX c;
	unsigned char digest[SHA256_DIGEST_LENGTH];
	char out[SHA256_DIGEST_LENGTH * 2 + 1];
	SHA256_Init(&c);
	unsigned char buffer[BUFFER_SIZE];
	size_t count = 0;
	while ((count = fread(buffer, 1, BUFFER_SIZE, infile)) > 0) {
		fwrite(buffer, 1, count, stdout);
		SHA256_Update(&c, buffer, count);
	}
	SHA256_Final(digest, &c);
	for (int n = 0; n < SHA256_DIGEST_LENGTH; ++n) {
		snprintf(&(out[n*2]), SHA256_DIGEST_LENGTH*2, "%02x", (unsigned int)digest[n]);
	}
	return std::string(out);
}

const std::string sha384_digest (FILE* infile, FILE* outfile) {
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
	return std::string(out);
}

const std::string sha512_digest (FILE* infile, FILE* outfile) {
	SHA512_CTX c;
	unsigned char digest[SHA512_DIGEST_LENGTH];
	char out[SHA512_DIGEST_LENGTH * 2 + 1];
	SHA512_Init(&c);
	unsigned char buffer[BUFFER_SIZE];
	size_t count = 0;
	while ((count = fread(buffer, 1, BUFFER_SIZE, infile)) > 0) {
		fwrite(buffer, 1, count, stdout);
		SHA512_Update(&c, buffer, count);
	}
	SHA512_Final(digest, &c);
	for (int n = 0; n < SHA512_DIGEST_LENGTH; ++n) {
		snprintf(&(out[n*2]), SHA512_DIGEST_LENGTH*2, "%02x", (unsigned int)digest[n]);
	}
	return std::string(out);
}

#endif
