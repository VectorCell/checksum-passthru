/*
* sump-thread.h
*/

#pragma once

#include <functional>
//#include <tuple>
#include <vector>
#include <string>

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <unistd.h>

#include <pthread.h>

#include <openssl/md5.h>
#include <openssl/sha.h>

#ifndef BUFFER_SIZE
#	define BUFFER_SIZE (1024U * 32U)
#else
#	error "BUFFER_SIZE already defined"
#endif

// shared data
uint8_t buffer[BUFFER_SIZE];
size_t n_bytes;
bool buffer_ready;

// monitors to control access to above shared data
pthread_cond_t write_cond, read_cond;
int n_writers, n_readers;
pthread_mutex_t lock;

enum digest_elem {
	DIGEST_NAME,
	DIGEST_LENGTH,
	DIGEST_CTX,
	DIGEST_INIT,
	DIGEST_UPDATE,
	DIGEST_FINAL
};
typedef std::function<int(void *)> init_fn;
typedef std::function<int(void *, const void *, size_t)> update_fn;
typedef std::function<int(uint8_t *, void *)> final_fn;
typedef struct digest_type_struct {
	std::string name;
	size_t length;
	void *ctx;
	init_fn init;
	update_fn update;
	final_fn final;
	uint32_t magic;
} digest_type;
//typedef std::tuple<std::string, size_t, void *, init_fn, update_fn, final_fn> digest_type;

#define DIGEST_MAGIC 0xdeadbeef

// reads
void *update (void *ptr) {
	digest_type *digest = (digest_type *)ptr;

	pthread_mutex_lock(&lock);
	while (n_writers > 0 || !buffer_ready)
		pthread_cond_wait(&write_cond, &lock);
	++n_readers;
	pthread_mutex_unlock(&lock);

	// do read here
	printf("reading buffer\n");
	digest->update(digest->ctx, buffer, n_bytes);

	pthread_mutex_lock(&lock);
	--n_readers;
	if (n_readers == 0) {
		buffer_ready = false;
		pthread_cond_signal(&read_cond);
	}
	pthread_mutex_unlock(&lock);
	return NULL;
}

bool fill_buffer (FILE *infile) {
	pthread_mutex_lock(&lock);
	++n_writers;
	while (n_readers > 0)
		pthread_cond_wait(&read_cond, &lock);

	// do write here
	printf("filling buffer");
	n_bytes = fread(buffer, 1, BUFFER_SIZE, infile);
	buffer_ready = true;

	--n_writers;
	if (n_bytes > 0)
		pthread_cond_broadcast(&write_cond);
	if (n_writers == 0)
		pthread_cond_signal(&read_cond);
	pthread_mutex_unlock(&lock);
	return n_bytes != 0;
}

template <typename C = std::vector<digest_type>>
void calc_digests (C digests, FILE *infile, FILE *outfile) {
	n_bytes = 0;
	buffer_ready = false;
	pthread_mutex_init(&lock, NULL);
	pthread_cond_init(&write_cond, NULL);
	pthread_cond_init(&read_cond, NULL);
	pthread_mutex_lock(&lock);

	std::vector<pthread_t> threads(digests.size());
	for (size_t k = 0; k < threads.size(); ++k) {
		pthread_create(&threads[k], NULL, update, &digests[k]);
	}

	// init
	for (digest_type& digest : digests) {
		digest.init(digest.ctx);
	}

	// update
	printf("filling buffer ...\n");
	while (fill_buffer(infile)) {
//		if (outfile != NULL)
//			fwrite(buffer, 1, n_bytes, outfile);
	}

	// final
	for (digest_type& digest : digests) {
		std::vector<uint8_t> bytes(digest.length);
		std::vector<char> printed(digest.length * 2 + 1);
		digest.final(&bytes[0], digest.ctx);
		for (size_t n = 0; n < digest.length; ++n)
			snprintf(&printed[n*2], digest.length * 2, "%02x", (unsigned int)bytes[n]);
		std::string str(&printed[0]);
		fprintf(outfile, "digest: %s %s\n", digest.name.c_str(), str.c_str());
	}

}

