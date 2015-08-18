CSTD   := c99
CPPSTD := c++11

ifeq "$(CXX)" "g++"
	GCCVERSIONLT48 := $(shell expr `gcc -dumpversion` \< 4.8)
	ifeq "$(GCCVERSIONLT48)" "1"
		CPPSTD := c++0x
	endif
endif

CFLAGS   := -pedantic -std=$(CSTD) -Wall -Werror -O3
CPPFLAGS := -pedantic -std=$(CPPSTD) -Wall -Werror -O3
LIBFLAGS := -lcrypto -lssl

all : sump md5sump sha1sump sha224sump sha256sump sha384sump sha512sump xxhsump

sump : sump.cc
	$(CXX) $(CPPFLAGS) -o sump sump.cc $(LIBFLAGS)

md5sump : md5sump.c
	$(CC) $(CFLAGS) -o md5sump md5sump.c $(LIBFLAGS)

sha1sump : sha1sump.c
	$(CC) $(CFLAGS) -o sha1sump sha1sump.c $(LIBFLAGS)

sha224sump : sha224sump.c
	$(CC) $(CFLAGS) -o sha224sump sha224sump.c $(LIBFLAGS)

sha256sump : sha256sump.c
	$(CC) $(CFLAGS) -o sha256sump sha256sump.c $(LIBFLAGS)

sha384sump : sha384sump.c
	$(CC) $(CFLAGS) -o sha384sump sha384sump.c $(LIBFLAGS)

sha512sump : sha512sump.c
	$(CC) $(CFLAGS) -o sha512sump sha512sump.c $(LIBFLAGS)

xxhsump : xxhsump.sh
	cp xxhsump.sh xxhsump

test : clean all
	./test.sh

install : all
	./install.sh

uninstall : all
	./uninstall.sh

clean :
	rm -f *.d
	rm -f *.o
	rm -f sump
	rm -f md5sump
	rm -f sha1sump
	rm -f sha224sump
	rm -f sha256sump
	rm -f sha384sump
	rm -f sha512sump
	rm -f xxhsump

-include *.d
