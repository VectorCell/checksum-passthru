/*
* sump.h
*/

#pragma once

#include <cstdio>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <functional>
#include <algorithm>
#include <exception>

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
	virtual int reset () = 0;
	virtual int update (const void *, size_t) = 0;
	virtual const std::string finalize () = 0;
	virtual ~AbstractDigest () = default;
};


class Digest {

private:

	AbstractDigest *_p;

public:

	Digest () : _p(nullptr) {};

	Digest (AbstractDigest *p) : _p(p) {};

	Digest (const Digest& d) = delete;

	Digest (Digest&& d) {
		std::swap(_p, d._p);
	}

	Digest& operator = (const Digest& d) = delete;

	~Digest () {
		delete _p;
	}

	int reset () {
		return _p->reset();
	}

	int update (const void *buffer, size_t size) {
		return _p->update(buffer, size);
	}

	const std::string finalize () {
		return _p->finalize();
	}
};


class CountDigest : public AbstractDigest {

private:

	size_t _size;

public:

	CountDigest () : _size() {}

	int reset () {
		_size = 0;
		return 0;
	}

	int update (const void *, size_t size) {
		_size += size;
		return 0;
	}

	const std::string finalize () {
		std::stringstream ss;
		ss << _size;
		return ss.str();
	}
};


template <typename DS, size_t DL>
class OpenSSLDigest : public AbstractDigest {

private:

	DS _c;
	fn_init<DS>   _init;
	fn_update<DS> _update;
	fn_final<DS>  _final;
	unsigned char digest[DL];

public:

	OpenSSLDigest (fn_init<DS> i, fn_update<DS> u, fn_final<DS> f) :
		_c(),
		_init(i),
		_update(u),
		_final(f) {
		_init(&_c);
	}

	int reset () {
		return _init(&_c);
	}

	int update (const void *buffer, size_t size) {
		return _update(&_c, buffer, size);
	}

	const std::string finalize () {
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
};


OpenSSLDigest<size_t,1>* build_digest_none () {
	return new OpenSSLDigest<size_t,1>(
		none_Init,
		none_Update,
		none_Final);
}

CountDigest* build_digest_count () {
	return new CountDigest;
}

OpenSSLDigest<MD5_CTX,MD5_DIGEST_LENGTH>* build_digest_md5 () {
	return new OpenSSLDigest<MD5_CTX,MD5_DIGEST_LENGTH>(
		MD5_Init,
		MD5_Update,
		MD5_Final);
}

OpenSSLDigest<SHA_CTX,SHA_DIGEST_LENGTH>* build_digest_sha1 () {
	return new OpenSSLDigest<SHA_CTX,SHA_DIGEST_LENGTH>(
		SHA1_Init,
		SHA1_Update,
		SHA1_Final);
}

OpenSSLDigest<SHA256_CTX,SHA224_DIGEST_LENGTH>* build_digest_sha224 () {
	return new OpenSSLDigest<SHA256_CTX,SHA224_DIGEST_LENGTH>(
		SHA224_Init,
		SHA224_Update,
		SHA224_Final);
}

OpenSSLDigest<SHA256_CTX,SHA256_DIGEST_LENGTH>* build_digest_sha256 () {
	return new OpenSSLDigest<SHA256_CTX,SHA256_DIGEST_LENGTH>(
		SHA256_Init,
		SHA256_Update,
		SHA256_Final);
}

OpenSSLDigest<SHA512_CTX,SHA384_DIGEST_LENGTH>* build_digest_sha384 () {
	return new OpenSSLDigest<SHA512_CTX,SHA384_DIGEST_LENGTH>(
		SHA384_Init,
		SHA384_Update,
		SHA384_Final);
}

OpenSSLDigest<SHA512_CTX,SHA512_DIGEST_LENGTH>* build_digest_sha512 () {
	return new OpenSSLDigest<SHA512_CTX,SHA512_DIGEST_LENGTH>(
		SHA512_Init,
		SHA512_Update,
		SHA512_Final);
}

AbstractDigest* build_digest (std::string name) {
	if (name == "none") {
		return build_digest_none();
	} else if (name == "count") {
		return build_digest_count();
	} else if (name == "md5") {
		return build_digest_md5();
	} else if (name == "sha1") {
		return build_digest_sha1();
	} else if (name == "sha224") {
		return build_digest_sha224();
	} else if (name == "sha256") {
		return build_digest_sha256();
	} else if (name == "sha384") {
		return build_digest_sha384();
	} else if (name == "sha512") {
		return build_digest_sha512();
	} else {
		return build_digest_none();
	}
}


size_t decode (char *str, int radix) {
	size_t num = 0;
	while (*str != '\0') {
		num *= radix;
		num += *str - '0';
		++str;
	}
	return num;
}


const size_t RECORD_SIZE = 512;
const size_t RECORDS_PER_READ = 8;
typedef struct tar_posix_header
{                       /* byte offset */
	char name[100];     /*   0 */
	char mode[8];       /* 100 */
	char uid[8];        /* 108 */
	char gid[8];        /* 116 */
	char size[12];      /* 124 */
	char mtime[12];     /* 136 */
	char chksum[8];     /* 148 */
	char typeflag;      /* 156 */
	char linkname[100]; /* 157 */
	char magic[6];      /* 257 */
	char version[2];    /* 263 */
	char uname[32];     /* 265 */
	char gname[32];     /* 297 */
	char devmajor[8];   /* 329 */
	char devminor[8];   /* 337 */
	char prefix[155];   /* 345 */
						/* 500 */
} tar_posix_header;


struct TARFileHeader {

	char filename[100]; //NUL-terminated
	char mode[8];
	char uid[8];
	char gid[8];
	char fileSize[12];
	char lastModification[12];
	char checksum[8];
	char typeFlag; //Also called link indicator for none-UStar format
	char linkedFileName[100];

	//USTar-specific fields -- NUL-filled in non-USTAR version
	char ustarIndicator[6]; //"ustar" -- 6th character might be NUL but results show it doesn't have to
	char ustarVersion[2]; //00
	char ownerUserName[32];
	char ownerGroupName[32];
	char deviceMajorNumber[8];
	char deviceMinorNumber[8];
	char filenamePrefix[155];
	char padding[12]; //Nothing of interest, but relevant for checksum

	/**
	 * @return true if and only if
	 */
	bool isUSTAR() {
		return (memcmp("ustar", ustarIndicator, 5) == 0);
	}

	/**
	 * @return The filesize in bytes
	 */
	size_t getFileSize() {
		return decode(fileSize, 8);
	}

	/**
	 * Return true if and only if the header checksum is correct
	 * @return
	 */
	bool checkChecksum() {
		//We need to set the checksum to zer
		char originalChecksum[8];
		memcpy(originalChecksum, checksum, 8);
		memset(checksum, ' ', 8);
		//Calculate the checksum -- both signed and unsigned
		size_t unsignedSum = 0;
		size_t signedSum = 0;
		for (unsigned i = 0; i < sizeof (TARFileHeader); i++) {
			unsignedSum += ((unsigned char*) this)[i];
			signedSum += ((signed char*) this)[i];
		}
		//Copy back the checksum
		memcpy(checksum, originalChecksum, 8);
		//Decode the original checksum
		size_t referenceChecksum = decode(originalChecksum, 8);
		return (referenceChecksum == unsignedSum || referenceChecksum == signedSum);
	}
};


class malformed_tar_error : public std::exception {

private:

	std::string _msg;

public:

	malformed_tar_error (const char *str) : _msg() {
		_msg = "malformed_tar_error: ";
		_msg += str;
	}

	const char* what () const noexcept {
		return _msg.c_str();
	}
};
