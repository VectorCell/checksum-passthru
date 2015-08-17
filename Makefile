CSTD   := c99
CPPSTD := c++11

GCCVERSIONLT48 := $(shell expr `gcc -dumpversion` \< 4.8)
ifeq "$(GCCVERSIONLT48)" "1"
	CPPSTD := c++0x
endif

CFLAGS   := -pedantic -std=$(CSTD) -Wall -Werror -O3
CPPFLAGS := -pedantic -std=$(CPPSTD) -Wall -Werror -O3
LIBFLAGS := -lcrypto -lssl

all : sumpass md5pass sha1pass sha224pass sha256pass sha384pass sha512pass xxhpass

sumpass : sumpass.cc
	$(CXX) $(CPPFLAGS) -o sumpass sumpass.cc $(LIBFLAGS)

md5pass : md5pass.c
	$(CC) $(CFLAGS) -o md5pass md5pass.c $(LIBFLAGS)

sha1pass : sha1pass.c
	$(CC) $(CFLAGS) -o sha1pass sha1pass.c $(LIBFLAGS)

sha224pass : sha224pass.c
	$(CC) $(CFLAGS) -o sha224pass sha224pass.c $(LIBFLAGS)

sha256pass : sha256pass.c
	$(CC) $(CFLAGS) -o sha256pass sha256pass.c $(LIBFLAGS)

sha384pass : sha384pass.c
	$(CC) $(CFLAGS) -o sha384pass sha384pass.c $(LIBFLAGS)

sha512pass : sha512pass.c
	$(CC) $(CFLAGS) -o sha512pass sha512pass.c $(LIBFLAGS)

xxhpass : xxhpass.sh
	cp xxhpass.sh xxhpass

test : clean all
	./test.sh

install : all
	./install.sh

uninstall : all
	./uninstall.sh

clean :
	rm -f *.d
	rm -f *.o
	rm -f sumpass
	rm -f md5pass
	rm -f sha1pass
	rm -f sha224pass
	rm -f sha256pass
	rm -f sha384pass
	rm -f sha512pass
	rm -f xxhpass

-include *.d
