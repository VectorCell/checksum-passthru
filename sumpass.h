/*
* sumpass.h
*/

#pragma once

#include <cstdio>
#include <string>
#include <vector>

#include <openssl/md5.h>
#include <openssl/sha.h>

#ifndef BUFFER_SIZE
#	define BUFFER_SIZE (1024U * 32U)
#else
#	error "BUFFER_SIZE already defined"
#endif

template <typename DS>
const std::string openssl_digest (FILE* infile, FILE* outfile, size_t digest_length, int(*init)(DS*), int(*update)(DS*, const void*, size_t), int(*final)(unsigned char*, DS*)) {
	DS c;
	unsigned char *digest = (unsigned char*)malloc(digest_length);
	std::vector<char> out(digest_length * 2 + 1);
	init(&c);
	unsigned char buffer[BUFFER_SIZE];
	size_t count = 0;
	while ((count = fread(buffer, 1, BUFFER_SIZE, infile)) > 0) {
		fwrite(buffer, 1, count, outfile);
		update(&c, buffer, count);
	}
	final(digest, &c);
	for (size_t n = 0; n < digest_length; ++n) {
		snprintf(&(out[n*2]), digest_length*2, "%02x", (unsigned int)digest[n]);
	}
	free(digest);
	return std::string(out.begin(), out.end());
}

const std::string md5_digest (FILE* infile, FILE* outfile) {
	return openssl_digest(infile, outfile, MD5_DIGEST_LENGTH, MD5_Init, MD5_Update, MD5_Final);
}

const std::string sha1_digest (FILE* infile, FILE* outfile) {
	return openssl_digest(infile, outfile, SHA_DIGEST_LENGTH, SHA1_Init, SHA1_Update, SHA1_Final);
}

const std::string sha224_digest (FILE* infile, FILE* outfile) {
	return openssl_digest(infile, outfile, SHA224_DIGEST_LENGTH, SHA224_Init, SHA224_Update, SHA224_Final);
}

const std::string sha256_digest (FILE* infile, FILE* outfile) {
	return openssl_digest(infile, outfile, SHA256_DIGEST_LENGTH, SHA256_Init, SHA256_Update, SHA256_Final);
}

const std::string sha384_digest (FILE* infile, FILE* outfile) {
	return openssl_digest(infile, outfile, SHA384_DIGEST_LENGTH, SHA384_Init, SHA384_Update, SHA384_Final);
}

const std::string sha512_digest (FILE* infile, FILE* outfile) {
	return openssl_digest(infile, outfile, SHA512_DIGEST_LENGTH, SHA512_Init, SHA512_Update, SHA512_Final);
}
