/*
* sump.h
*/

#pragma once

#include <cstdio>
#include <string>
#include <vector>

#include <openssl/md5.h>
#include <openssl/sha.h>

#include <byteswap.h> // for swapping endian-ness

#ifndef BUFFER_SIZE
#	define BUFFER_SIZE (1024U * 32U)
#else
#	error "BUFFER_SIZE already defined"
#endif

int no_Init (size_t*) {
	return 0;
}
int no_Update (size_t*, const void*, size_t) {
	return 0;
}
int no_Final (unsigned char*, size_t*) {
	return 0;
}

int xor32_Init (uint32_t *bytes) {
	*bytes = 0;
	return 0;
}
int xor32_Update (uint32_t *bytes, const void *buf, size_t cnt) {
	if (cnt % sizeof(uint32_t) == 0) {
		const uint32_t *ptr = (const uint32_t *)buf;
		for (size_t k = 0; k < cnt; k += sizeof(uint32_t)) {
			*bytes ^= *(uint32_t *)ptr;
			++ptr;
		}
	} else {
		const char *buffer = (const char *)buf;
		for (size_t k = 0; k < cnt; ++k) {
			uint32_t mask = (uint32_t)buffer[k] << (8 * (k % sizeof(uint32_t)));
			*bytes ^= mask;
		}
	}
	return 0;
}
int xor32_Final (unsigned char *out, uint32_t *bytes) {
	*bytes = __bswap_32(*bytes);
	*(uint32_t *)out = *bytes;
	return 0;
}

int xor64_Init (uint64_t *bytes) {
	*bytes = 0;
	return 0;
}
int xor64_Update (uint64_t *bytes, const void *buf, size_t cnt) {
	if (cnt % sizeof(uint64_t) == 0) {
		const uint64_t *ptr = (const uint64_t *)buf;
		for (size_t k = 0; k < cnt; k += sizeof(uint64_t)) {
			*bytes ^= *(uint64_t *)ptr;
			++ptr;
		}
	} else {
		const char *buffer = (const char *)buf;
		for (size_t k = 0; k < cnt; ++k) {
			uint64_t mask = (uint64_t)buffer[k] << (8 * (k % sizeof(uint64_t)));
			*bytes ^= mask;
		}
	}
	return 0;
}
int xor64_Final (unsigned char *out, uint64_t *bytes) {
	*bytes = __bswap_64(*bytes);
	*(uint64_t *)out = *bytes;
	return 0;
}

typedef __uint128_t uint128_t;
int xor128_Init (uint128_t *bytes) {
	*bytes = 0;
	return 0;
}
int xor128_Update (uint128_t *bytes, const void *buf, size_t cnt) {
	if (cnt % sizeof(uint128_t) == 0) {
		const uint128_t *ptr = (const uint128_t *)buf;
		for (size_t k = 0; k < cnt; k += sizeof(uint128_t)) {
			*bytes ^= *(uint128_t *)ptr;
			++ptr;
		}
	} else {
		const char *buffer = (const char *)buf;
		for (size_t k = 0; k < cnt; ++k) {
			uint128_t mask = (uint128_t)buffer[k] << (8 * (k % sizeof(uint128_t)));
			*bytes ^= mask;
		}
	}
	return 0;
}
int xor128_Final (unsigned char *out, uint128_t *bytes) {
	uint64_t *split = (uint64_t *)bytes;
	split[0] = __bswap_64(split[0]);
	split[1] = __bswap_64(split[1]);
	*(uint64_t *)out = split[1];
	*(uint64_t *)(out + sizeof(uint64_t)) = split[0];
	return 0;
}

template <typename DS = size_t, size_t DIGEST_LENGTH = 1>
std::string digest (FILE* infile, 
	                FILE* outfile, 
	                int(*init)(DS*) = no_Init, 
	                int(*update)(DS*, const void*, size_t) = no_Update, 
	                int(*final)(unsigned char*, DS*) = no_Final) {
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

std::string no_digest (FILE *infile, FILE *outfile) {
	return digest(infile, outfile);
}

std::string md5_digest (FILE *infile, FILE *outfile) {
	return digest<MD5_CTX, MD5_DIGEST_LENGTH>
		(infile, outfile, MD5_Init, MD5_Update, MD5_Final);
}

std::string sha1_digest (FILE *infile, FILE *outfile) {
	return digest<SHA_CTX, SHA_DIGEST_LENGTH>
		(infile, outfile, SHA1_Init, SHA1_Update, SHA1_Final);
}

std::string sha224_digest (FILE *infile, FILE *outfile) {
	return digest<SHA256_CTX, SHA224_DIGEST_LENGTH>
		(infile, outfile, SHA224_Init, SHA224_Update, SHA224_Final);
}

std::string sha256_digest (FILE *infile, FILE *outfile) {
	return digest<SHA256_CTX, SHA256_DIGEST_LENGTH>
		(infile, outfile, SHA256_Init, SHA256_Update, SHA256_Final);
}

std::string sha384_digest (FILE *infile, FILE *outfile) {
	return digest<SHA512_CTX, SHA384_DIGEST_LENGTH>
		(infile, outfile, SHA384_Init, SHA384_Update, SHA384_Final);
}

std::string sha512_digest (FILE *infile, FILE *outfile) {
	return digest<SHA512_CTX, SHA512_DIGEST_LENGTH>
		(infile, outfile, SHA512_Init, SHA512_Update, SHA512_Final);
}

std::string xor32_digest (FILE *infile, FILE *outfile) {
	return digest<uint32_t, sizeof(uint32_t)>
		(infile, outfile, xor32_Init, xor32_Update, xor32_Final);
}

std::string xor64_digest (FILE *infile, FILE *outfile) {
	return digest<uint64_t, sizeof(uint64_t)>
		(infile, outfile, xor64_Init, xor64_Update, xor64_Final);
}

std::string xor128_digest (FILE *infile, FILE *outfile) {
	return digest<uint128_t, sizeof(uint128_t)>
		(infile, outfile, xor128_Init, xor128_Update, xor128_Final);
}
