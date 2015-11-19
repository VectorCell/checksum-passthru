/*
* sump.cc
*/

#include <iostream>
#include <functional>

#include <cstdio>
#include <unistd.h>
#include <cstring>
#include <string>
#include <vector>

#include <openssl/md5.h>
#include <openssl/sha.h>

#include <byteswap.h> // for swapping endian-ness

#include "sump.h"

using namespace std;

typedef function<string(FILE*, FILE*)> digest_fn;

//                                  . ####:   . ####:  
//                                  #######:  #######: 
//                                  #:.   ##  #:.   ## 
//    ###  ###   .####.    ##.####        ##        ## 
//     ##::##   .######.   #######        ##       :#  
//     :####:   ###  ###   ###.       #####        ##  
//      ####    ##.  .##   ##         #####.     .##:  
//      :##:    ##    ##   ##             ##    .##:   
//      ####    ##.  .##   ##             ##   :##:    
//     :####:   ###  ###   ##       #:    ##  :##:     
//     ##::##   .######.   ##       #######:  ######## 
//    ###  ###   .####.    ##       :#####:   ######## 

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

//                                     ###:       ###  
//                                   ######      :###  
//                                  :##. .#     .####  
//    ###  ###   .####.    ##.####  ##:         ##.##  
//     ##::##   .######.   #######  ##:###:    :#: ##  
//     :####:   ###  ###   ###.     #######:  .##  ##  
//      ####    ##.  .##   ##       ##    ##  ##   ##  
//      :##:    ##    ##   ##       ##    ##  ######## 
//      ####    ##.  .##   ##       ##    ##  ######## 
//     :####:   ###  ###   ##        #    ##       ##  
//     ##::##   .######.   ##        ######:       ##  
//    ###  ###   .####.    ##        .####:        ##  

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

//                                   .###     . ####:    :####:  
//                                   ####     #######:  :######: 
//                                   #:##     #:.   ##  ##    ## 
//    ###  ###   .####.    ##.####     ##           ##  ##    ## 
//     ##::##   .######.   #######     ##          :#   ##    ## 
//     :####:   ###  ###   ###.        ##          ##    ######  
//      ####    ##.  .##   ##          ##        .##:   .######. 
//      :##:    ##    ##   ##          ##       .##:    ##    ## 
//      ####    ##.  .##   ##          ##      :##:     ##    ## 
//     :####:   ###  ###   ##          ##     :##:      ##    ## 
//     ##::##   .######.   ##       ########  ########  :######: 
//    ###  ###   .####.    ##       ########  ########   :####:  

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

//                                  ##                 
//                                  ##                 
//                                  ##                 
//       ####:   .####.   ## #:##:  ##.###:    .####.  
//     #######  .######.  ########  #######:  .######. 
//     ##:  :#  ###  ###  ##.##.##  ###  ###  ###  ### 
//    ##.       ##.  .##  ## ## ##  ##.  .##  ##.  .## 
//    ##        ##    ##  ## ## ##  ##    ##  ##    ## 
//    ##.       ##.  .##  ## ## ##  ##.  .##  ##.  .## 
//     ##:  .#  ###  ###  ## ## ##  ###  ###  ###  ### 
//     #######  .######.  ## ## ##  #######:  .######. 
//       ####:   .####.   ## ## ##  ##.###:    .####.  

const size_t COMBO_LENGTH = MD5_DIGEST_LENGTH
	                      + SHA_DIGEST_LENGTH
	                      + SHA224_DIGEST_LENGTH
	                      + SHA256_DIGEST_LENGTH
	                      + SHA384_DIGEST_LENGTH
	                      + SHA512_DIGEST_LENGTH;

typedef struct combo_ctx {
	MD5_CTX    md5_ctx;
	SHA_CTX    sha1_ctx;
	SHA256_CTX sha224_ctx;
	SHA256_CTX sha256_ctx;
	SHA512_CTX sha384_ctx;
	SHA512_CTX sha512_ctx;
} combo_ctx;

int combo_Init (combo_ctx *ctx) {
	MD5_Init(&ctx->md5_ctx);
	SHA_Init(&ctx->sha1_ctx);
	SHA224_Init(&ctx->sha224_ctx);
	SHA256_Init(&ctx->sha256_ctx);
	SHA384_Init(&ctx->sha384_ctx);
	SHA512_Init(&ctx->sha512_ctx);
	return 0;
}
int combo_Update (combo_ctx *ctx, const void *buf, size_t cnt) {
	MD5_Update(&ctx->md5_ctx, buf, cnt);
	SHA_Update(&ctx->sha1_ctx, buf, cnt);
	SHA224_Update(&ctx->sha224_ctx, buf, cnt);
	SHA256_Update(&ctx->sha256_ctx, buf, cnt);
	SHA384_Update(&ctx->sha384_ctx, buf, cnt);
	SHA512_Update(&ctx->sha512_ctx, buf, cnt);
	return 0;
}
int combo_Final (unsigned char *out, combo_ctx *ctx) {
	MD5_Final(out, &ctx->md5_ctx);
	out += MD5_DIGEST_LENGTH;
	SHA_Final(out, &ctx->sha1_ctx);
	out += SHA_DIGEST_LENGTH;
	SHA224_Final(out, &ctx->sha224_ctx);
	out += SHA224_DIGEST_LENGTH;
	SHA256_Final(out, &ctx->sha256_ctx);
	out += SHA256_DIGEST_LENGTH;
	SHA384_Final(out, &ctx->sha384_ctx);
	out += SHA384_DIGEST_LENGTH;
	SHA512_Final(out, &ctx->sha512_ctx);
	return 0;
}

//                 ##                                                      
//          ##     ##                                                      
//          ##     ##                                     ##               
//          ##                                            ##               
//     :###.##   ####      :###:##   .####:    :#####.  #######    :#####. 
//    :#######   ####     .#######  .######:  ########  #######   ######## 
//    ###  ###     ##     ###  ###  ##:  :##  ##:  .:#    ##      ##:  .:# 
//    ##.  .##     ##     ##.  .##  ########  ##### .     ##      ##### .  
//    ##    ##     ##     ##    ##  ########  .######:    ##      .######: 
//    ##.  .##     ##     ##.  .##  ##           .: ##    ##         .: ## 
//    ###  ###     ##     ###  ###  ###.  :#  #:.  :##    ##.     #:.  :## 
//    :#######  ########  .#######  .#######  ########    #####   ######## 
//     :###.##  ########   :###:##   .#####:  . ####      .####   . ####   
//                         #.  :##                                         
//                         ######                                          
//                         :####:                                          

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

std::string combo_digest (FILE *infile, FILE *outfile) {
	return digest<combo_ctx, COMBO_LENGTH>
		(infile, outfile, combo_Init, combo_Update, combo_Final);
}

digest_fn get_digest_fn(string& alg) {
	digest_fn df = no_digest;
	if (alg == "md5") {
		df = md5_digest;
	} else if (alg == "sha1") {
		df = sha1_digest;
	} else if (alg == "sha224") {
		df = sha224_digest;
	} else if (alg == "sha256") {
		df = sha256_digest;
	} else if (alg == "sha384") {
		df = sha384_digest;
	} else if (alg == "sha512") {
		df = sha512_digest;
	} else if (alg == "xor32") {
		df = xor32_digest;
	} else if (alg == "xor64") {
		df = xor64_digest;
	} else if (alg == "xor128") {
		df = xor128_digest;
	} else if (alg == "combo") {
		df = combo_digest;
	} else {
		alg = "none";
	}
	return df;
}

//                           ##              
//                           ##              
//
//    ## #:##:   :####     ####     ##.####  
//    ########   ######    ####     #######  
//    ##.##.##   #:  :##     ##     ###  :## 
//    ## ## ##    :#####     ##     ##    ## 
//    ## ## ##  .#######     ##     ##    ## 
//    ## ## ##  ## .  ##     ##     ##    ## 
//    ## ## ##  ##:  ###     ##     ##    ## 
//    ## ## ##  ########  ########  ##    ## 
//    ## ## ##    ###.##  ########  ##    ## 

int main (int argc, char *argv[]) {
	digest_fn df;
	string alg = "md5";
	FILE* infile;
	FILE* outfile;
	FILE* sumfile;
	string infile_name = "-";
	string outfile_name = "-";
	string sumfile_name = "-";
	bool is_infile = false;
	bool is_outfile = false;
	bool is_sumfile = false;
	for (int k = 0; k < argc; ++k) {
		if (is_infile) {
			is_infile = false;
			infile_name = argv[k];
		} else if (is_outfile) {
			is_outfile = false;
			outfile_name = argv[k];
		} else if (is_sumfile) {
			is_sumfile = false;
			sumfile_name = argv[k];
		} else {
			if (!strcmp("-i", argv[k])) {
				is_infile = true;
			} else if (!strcmp("-o", argv[k])) {
				is_outfile = true;
			} else if (!strcmp("-s", argv[k])) {
				is_sumfile = true;
			} else {
				alg = argv[k];
			}
		}
	}

	if (infile_name == "-") {
		infile = stdin;
	} else {
		infile = fopen(infile_name.c_str(), "r");
	}

	if (outfile_name == "-") {
		if (isatty(fileno(stdout))) {
			outfile = NULL;
		} else {
			outfile = stdout;
		}
	} else {
		outfile = fopen(outfile_name.c_str(), "w");
	}

	if (sumfile_name == "-") {
		sumfile = stderr;
	} else {
		sumfile = fopen(sumfile_name.c_str(), "w");
	}

	df = get_digest_fn(alg);
	if (alg == "none") {
		cout << "ERROR: must specifiy valid hash algorithm" << endl;
		exit(1);
	}
	string digest = df(infile, outfile);
	fprintf(sumfile, "%s  %s\n", digest.c_str(), infile_name.c_str());
	return 0;
}
