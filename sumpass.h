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
