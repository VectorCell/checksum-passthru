# checksum-passthru

Tool for getting the checksum of piped input while it's in use.

Use the same way you'd use md5sum-type programs without arguments (getting data from stdin), but
unlike md5sum-type programs, checksum-passthru outputs to stderr and passes input from stdin to stdout.

For example, you could use it to verify a file as it's being downloaded:
```
curl http://url/image.iso | md5pass > image.iso 2> image.iso.md5
```

As a side note, performance seems to be superior to corresponding md5sum-type programs for large data streams.

Benchmarked with `benchmark.sh` using an 1 GB of test data on an Intel Xeon E3-1271 @ 3.6 GHz in Ubuntu 14.04.3 LTS, built with GCC 4.8.

| Hash   | *sum     | *pass    |
|--------|----------|----------|
| MD5    | 671 MB/s | 709 MB/s |
| SHA1   | 426 MB/s | 826 MB/s |
| SHA224 | 244 MB/s | 292 MB/s |
| SHA256 | 243 MB/s | 292 MB/s |
| SHA384 | 365 MB/s | 447 MB/s |
| SHA512 | 363 MB/s | 445 MB/s |

xxhpass is included as a shell script for convenience, but it just piggy-backs on [xxhsum](https://github.com/Cyan4973/xxHash),
so it suffers a bit of a performance penalty (less pronounced for large data sets).
It's still way faster than any of the other hashing programs, though.

Travis CI builds:

|Branch      |Status   |
|------------|---------|
|master      | [![Build Status](https://travis-ci.org/VectorCell/checksum-passthru.svg?branch=master)](https://travis-ci.org/VectorCell/checksum-passthru?branch=master) |
