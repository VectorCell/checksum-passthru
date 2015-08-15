CXXFLAGS := -pedantic -std=c99 -Wall
LIBFLAGS := -lcrypto -lssl
VALGRIND := valgrind

md5-pass : md5-pass.o
	$(CC) $(CXXFLAGS) -o md5-pass md5-pass.o $(LIBFLAGS)

%.o : %.c
	$(CC) $(CXXFLAGS) -MD -c $*.c

test : clean md5-pass
	echo "hello world" | $(VALGRIND) ./md5-pass | md5sum
	dd if=/dev/urandom bs=1k count=1k 2> /dev/null | $(VALGRIND) ./md5-pass | md5sum
	$(VALGRIND) ./md5-pass < /usr/share/dict/words | md5sum

clean :
	rm -f *.d
	rm -f *.o
	rm -f md5-pass

-include *.d
