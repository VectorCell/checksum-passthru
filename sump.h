/*
* sump.h
*/

#pragma once

#include <cstdio>
#include <string>
#include <functional>

#ifndef BUFFER_SIZE
#	define BUFFER_SIZE (1024U * 32U)
#else
#	error "BUFFER_SIZE already defined"
#endif


int no_Init (size_t *) {
	return 0;
}
int no_Update (size_t *, const void *, size_t) {
	return 0;
}
int no_Final (unsigned char *, size_t*) {
	return 0;
}

template <typename DS = size_t, size_t DIGEST_LENGTH = 1>
std::string digest (FILE* infile, 
	                FILE* outfile, 
	                int(*init)(DS*) = no_Init, 
	                int(*update)(DS*, const void *, size_t) = no_Update, 
	                int(*final)(unsigned char *, DS*) = no_Final) {
	DS c;
	unsigned char digest[DIGEST_LENGTH];
	char out[DIGEST_LENGTH * 2 + 1];
	init(&c);
	unsigned char buffer[BUFFER_SIZE];
	size_t count = 0;
	while ((count = fread(buffer, 1, BUFFER_SIZE, infile)) > 0) {
		if (outfile != NULL)
			fwrite(buffer, 1, count, outfile);
		update(&c, buffer, count);
	}
	final(digest, &c);
	for (size_t n = 0; n < DIGEST_LENGTH; ++n) {
		snprintf(&(out[n*2]), DIGEST_LENGTH*2, "%02x", (unsigned int)digest[n]);
	}
	return std::string(out);
}
