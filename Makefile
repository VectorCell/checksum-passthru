CXXFLAGS := -pedantic -std=c99 -Wall
LIBFLAGS := -lcrypto -lssl
VALGRIND := valgrind

all : md5pass sha1pass sha224pass sha256pass sha384pass sha512pass

md5pass : md5pass.o
	$(CC) $(CXXFLAGS) -o md5pass md5pass.o $(LIBFLAGS)

sha1pass : sha1pass.o
	$(CC) $(CXXFLAGS) -o sha1pass sha1pass.o $(LIBFLAGS)

sha224pass : sha224pass.o
	$(CC) $(CXXFLAGS) -o sha224pass sha224pass.o $(LIBFLAGS)

sha256pass : sha256pass.o
	$(CC) $(CXXFLAGS) -o sha256pass sha256pass.o $(LIBFLAGS)

sha384pass : sha384pass.o
	$(CC) $(CXXFLAGS) -o sha384pass sha384pass.o $(LIBFLAGS)

sha512pass : sha512pass.o
	$(CC) $(CXXFLAGS) -o sha512pass sha512pass.o $(LIBFLAGS)

%.o : %.c
	$(CC) $(CXXFLAGS) -MD -c $*.c

test : clean all
	echo "hello world" | $(VALGRIND) ./md5pass | md5sum
	dd if=/dev/urandom bs=1k count=1k 2> /dev/null | $(VALGRIND) ./md5pass | md5sum
	$(VALGRIND) ./md5pass < /usr/share/dict/words | md5sum

	echo "hello world" | $(VALGRIND) ./sha1pass | sha1sum
	dd if=/dev/urandom bs=1k count=1k 2> /dev/null | $(VALGRIND) ./sha1pass | sha1sum
	$(VALGRIND) ./sha1pass < /usr/share/dict/words | sha1sum
	
	echo "hello world" | $(VALGRIND) ./sha224pass | sha224sum
	dd if=/dev/urandom bs=1k count=1k 2> /dev/null | $(VALGRIND) ./sha224pass | sha224sum
	$(VALGRIND) ./sha224pass < /usr/share/dict/words | sha224sum
	
	echo "hello world" | $(VALGRIND) ./sha256pass | sha256sum
	dd if=/dev/urandom bs=1k count=1k 2> /dev/null | $(VALGRIND) ./sha256pass | sha256sum
	$(VALGRIND) ./sha256pass < /usr/share/dict/words | sha256sum
	
	echo "hello world" | $(VALGRIND) ./sha384pass | sha384sum
	dd if=/dev/urandom bs=1k count=1k 2> /dev/null | $(VALGRIND) ./sha384pass | sha384sum
	$(VALGRIND) ./sha384pass < /usr/share/dict/words | sha384sum
	
	echo "hello world" | $(VALGRIND) ./sha512pass | sha512sum
	dd if=/dev/urandom bs=1k count=1k 2> /dev/null | $(VALGRIND) ./sha512pass | sha512sum
	$(VALGRIND) ./sha512pass < /usr/share/dict/words | sha512sum

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
