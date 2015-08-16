CFLAGS := -pedantic -std=c99 -Wall -O3
LIBFLAGS := -lcrypto -lssl

all : md5pass sha1pass sha224pass sha256pass sha384pass sha512pass

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

test : clean all
	./test.sh
	./benchmark.sh

install : all
	./install.sh

uninstall : all
	./uninstall.sh

clean :
	rm -f *.d
	rm -f *.o
	rm -f md5pass
	rm -f sha1pass
	rm -f sha224pass
	rm -f sha256pass
	rm -f sha384pass
	rm -f sha512pass

-include *.d
