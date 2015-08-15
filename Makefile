CXXFLAGS := -pedantic -std=c99 -Wall
LIBFLAGS := -lcrypto -lssl

md5-pass : md5-pass.o Makefile
	$(CC) $(CXXFLAGS) -o md5-pass md5-pass.o $(LIBFLAGS)

%.o : %.c Makefile
	$(CC) $(CXXFLAGS) -MD -c $*.c

test : clean Makefile md5-pass
	echo "hello world" | ./md5-pass

clean :
	rm -f *.d
	rm -f *.o
	rm -f md5-pass

-include *.d
