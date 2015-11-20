/*
* sump-thread.h
*/

#pragma once

#include <functional>
#include <tuple>
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
typedef std::tuple<std::string, size_t, void *, init_fn, update_fn, final_fn> digest_type;

typedef struct updater {
	pthread_mutex_t lock;
	void *ctx;
	uint8_t *buf;
	size_t cnt;
	size_t dl;
} updater;

void *update (void *ptr) {
	updater *u = (updater *)ptr;
	pthread_mutex_lock(&u->lock);
	pthread_mutex_unlock(&u->lock);
	return NULL;
}

std::string final (void *ptr) {
	return "not implemented";
}

template <typename C = std::vector<digest_type>>
void calc_digests (C digests, FILE *infile, FILE *outfile) {

	pthread_mutex_t lock;
	pthread_mutex_init(&lock, NULL);
	pthread_mutex_lock(&lock);

	uint8_t buffer[BUFFER_SIZE];

	std::vector<pthread_t> threads(digests.size());
	std::vector<updater> updaters(digests.size());
	for (size_t k = 0; k < threads.size(); ++k) {
		pthread_mutex_init(&updaters[k].lock, NULL);
		pthread_mutex_lock(&updaters[k].lock);
		updaters[k].ctx = std::get<DIGEST_CTX>(digests[k]);
		updaters[k].buf = buffer;
		updaters[k].cnt = 0;
		updaters[k].dl = std::get<DIGEST_LENGTH>(digests[k]);
		pthread_create(&threads[k], NULL, update, &updaters[k]);
	}

	// init
	for (digest_type digest : digests) {
		init_fn& init_digest = std::get<DIGEST_INIT>(digest);
		init_digest(std::get<DIGEST_CTX>(digest));
	}

	// final
	for (digest_type digest : digests) {
		std::string str = final(std::get<DIGEST_CTX>(digest));
		fprintf(outfile, "digest: %s %s\n",
			std::get<DIGEST_NAME>(digest).c_str(),
			str.c_str());
	}

}

