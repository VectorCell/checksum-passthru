CFLAGS := -pedantic -std=c99 -Wall -O3
LIBFLAGS := -lcrypto -lssl

all : md5pass sha1pass sha224pass sha256pass sha384pass sha512pass

md5pass : md5pass.o
	$(CC) $(CFLAGS) -o md5pass md5pass.o $(LIBFLAGS)

sha1pass : sha1pass.o
	$(CC) $(CFLAGS) -o sha1pass sha1pass.o $(LIBFLAGS)

sha224pass : sha224pass.o
	$(CC) $(CFLAGS) -o sha224pass sha224pass.o $(LIBFLAGS)

sha256pass : sha256pass.o
	$(CC) $(CFLAGS) -o sha256pass sha256pass.o $(LIBFLAGS)

sha384pass : sha384pass.o
	$(CC) $(CFLAGS) -o sha384pass sha384pass.o $(LIBFLAGS)

sha512pass : sha512pass.o
	$(CC) $(CFLAGS) -o sha512pass sha512pass.o $(LIBFLAGS)

%.o : %.c
	$(CC) $(CFLAGS) -MD -c $*.c

test : clean all
	./test.sh

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
