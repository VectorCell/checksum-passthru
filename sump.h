/*
* sump.h
*/

#pragma once

#include <cstdio>
#include <string>
#include <vector>
#include <map>
#include <functional>

#include <openssl/md5.h>
#include <openssl/sha.h>

#ifndef BUFFER_SIZE
#	define BUFFER_SIZE (1024U * 32U)
#else
#	error "BUFFER_SIZE already defined"
#endif


template <typename DS = size_t>
using fn_init = std::function<int(DS*)>;

template <typename DS = size_t>
using fn_update = std::function<int(DS*, const void*, size_t)>;

template <typename DS = size_t>
using fn_final = std::function<int(unsigned char*, DS*)>;

int none_Init (size_t*) {
	return 0;
}
int none_Update (size_t*, const void*, size_t) {
	return 0;
}
int none_Final (unsigned char*, size_t*) {
	return 0;
}

class AbstractDigest {
public:
	virtual int update (const void *, size_t) = 0;
	virtual const std::string finalize () = 0;
	virtual ~AbstractDigest () = default;
};

template <typename DS, size_t DL>
class Digest : public AbstractDigest {

private:

	DS _c;
	fn_init<DS>   _init;
	fn_update<DS> _update;
	fn_final<DS>  _final;
	unsigned char digest[DL];

	std::string get_string () {
		unsigned char digest[DL];
		_final(digest, &_c);
		char out[DL * 2 + 1];
		for (size_t n = 0; n < DL; ++n) {
			snprintf(&(out[n * 2]),
			         DL * 2,
			         "%02x",
			         static_cast<unsigned int>(digest[n]));
		}
		return std::string(out);
	}

public:

	Digest (fn_init<DS> i, fn_update<DS> u, fn_final<DS> f) :
		_c(),
		_init(i),
		_update(u),
		_final(f) {
		_init(&_c);
	}

	int update (const void *buffer, size_t size) {
		return _update(&_c, buffer, size);
	}

	const std::string finalize () {
		static const std::string str = get_string();
		return str;
	}
};

Digest<size_t,1>* build_digest_none () {
	return new Digest<size_t,1>(
		none_Init,
		none_Update,
		none_Final);
}

Digest<MD5_CTX,MD5_DIGEST_LENGTH>* build_digest_md5 () {
	return new Digest<MD5_CTX,MD5_DIGEST_LENGTH>(
		MD5_Init,
		MD5_Update,
		MD5_Final);
}

Digest<SHA_CTX,SHA_DIGEST_LENGTH>* build_digest_sha1 () {
	return new Digest<SHA_CTX,SHA_DIGEST_LENGTH>(
		SHA1_Init,
		SHA1_Update,
		SHA1_Final);
}

Digest<SHA256_CTX,SHA224_DIGEST_LENGTH>* build_digest_sha224 () {
	return new Digest<SHA256_CTX,SHA224_DIGEST_LENGTH>(
		SHA224_Init,
		SHA224_Update,
		SHA224_Final);
}

Digest<SHA256_CTX,SHA256_DIGEST_LENGTH>* build_digest_sha256 () {
	return new Digest<SHA256_CTX,SHA256_DIGEST_LENGTH>(
		SHA256_Init,
		SHA256_Update,
		SHA256_Final);
}

Digest<SHA512_CTX,SHA384_DIGEST_LENGTH>* build_digest_sha384 () {
	return new Digest<SHA512_CTX,SHA384_DIGEST_LENGTH>(
		SHA384_Init,
		SHA384_Update,
		SHA384_Final);
}

Digest<SHA512_CTX,SHA512_DIGEST_LENGTH>* build_digest_sha512 () {
	return new Digest<SHA512_CTX,SHA512_DIGEST_LENGTH>(
		SHA512_Init,
		SHA512_Update,
		SHA512_Final);
}
