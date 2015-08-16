# checksum-passthru

Tool for getting the md5sum of piped input while it's in use.

Use the same way you'd use md5sum-type programs without arguments (getting data from stdin), but
unlike md5sum-type programs, checksum-passthru outputs to stderr and passes input from stdin to stdout.

For example, you could use it to verify a file as it's being downloaded:
```
curl http://url/image.iso | md5pass > image.iso 2> image.iso.md5
```

As a side note, performance seems to be superior to corresponding md5sum-type programs.



Travis CI builds:

|Branch      |Status   |
|------------|---------|
|master      | [![Build Status](https://travis-ci.org/VectorCell/checksum-passthru.svg?branch=master)](https://travis-ci.org/VectorCell/checksum-passthru?branch=master) |
