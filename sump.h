/*
* sump.h
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

std::string no_digest (FILE* infile, FILE* outfile) {
	unsigned char buffer[BUFFER_SIZE];
	size_t count = 0;
	while ((count = fread(buffer, 1, BUFFER_SIZE, infile)) > 0) {
		fwrite(buffer, 1, count, outfile);
	}
	return "no algorithm applied";
}

template <typename DS, size_t DIGEST_LENGTH>
std::string openssl_digest (FILE* infile, FILE* outfile, int(*init)(DS*), int(*update)(DS*, const void*, size_t), int(*final)(unsigned char*, DS*)) {
	DS c;
	unsigned char digest[DIGEST_LENGTH];
	char out[DIGEST_LENGTH * 2 + 1];
	init(&c);
	unsigned char buffer[BUFFER_SIZE];
	size_t count = 0;
	while ((count = fread(buffer, 1, BUFFER_SIZE, infile)) > 0) {
		fwrite(buffer, 1, count, outfile);
		update(&c, buffer, count);
	}
	final(digest, &c);
	for (size_t n = 0; n < DIGEST_LENGTH; ++n) {
		snprintf(&(out[n*2]), DIGEST_LENGTH*2, "%02x", (unsigned int)digest[n]);
	}
	return std::string(out);
}

std::string md5_digest (FILE* infile, FILE* outfile) {
	return openssl_digest<MD5_CTX,MD5_DIGEST_LENGTH>(infile, outfile, MD5_Init, MD5_Update, MD5_Final);
}

std::string sha1_digest (FILE* infile, FILE* outfile) {
	return openssl_digest<SHA_CTX,SHA_DIGEST_LENGTH>(infile, outfile, SHA1_Init, SHA1_Update, SHA1_Final);
}

std::string sha224_digest (FILE* infile, FILE* outfile) {
	return openssl_digest<SHA256_CTX,SHA224_DIGEST_LENGTH>(infile, outfile, SHA224_Init, SHA224_Update, SHA224_Final);
}

std::string sha256_digest (FILE* infile, FILE* outfile) {
	return openssl_digest<SHA256_CTX,SHA256_DIGEST_LENGTH>(infile, outfile, SHA256_Init, SHA256_Update, SHA256_Final);
}

std::string sha384_digest (FILE* infile, FILE* outfile) {
	return openssl_digest<SHA512_CTX,SHA384_DIGEST_LENGTH>(infile, outfile, SHA384_Init, SHA384_Update, SHA384_Final);
}

std::string sha512_digest (FILE* infile, FILE* outfile) {
	return openssl_digest<SHA512_CTX,SHA512_DIGEST_LENGTH>(infile, outfile, SHA512_Init, SHA512_Update, SHA512_Final);
}
