# md5-pass

Tool for getting the md5sum of piped input while it's in use.

Use the same way you'd use md5sum without arguments (getting data from stdin), but 
unlike md5sum, md5-pass outputs to stderr and passes input from stdin to stdout.

For example, you could use it to verify a file as it's being downloaded:
```
curl http://url/linux-distro.iso | md5-pass > linux-distro.iso 2> linux-distro.iso.md5
```

Travis CI builds:

|Branch      |Status   |
|------------|---------|
|master      | [![Build Status](https://travis-ci.org/VectorCell/md5-pass.svg?branch=master)](https://travis-ci.org/VectorCell/md5-pass?branch=master) |
