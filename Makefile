CPPFLAGS := -pedantic -std=c++0x -Wall -O3
LIBFLAGS := -lcrypto -lssl

all : sumpass md5pass sha1pass sha224pass sha256pass sha384pass sha512pass xxhpass

sumpass : sumpass.cc
	$(CXX) $(CPPFLAGS) -o sumpass sumpass.cc $(LIBFLAGS)

md5pass : md5pass.cc
	$(CXX) $(CPPFLAGS) -o md5pass md5pass.cc $(LIBFLAGS)

sha1pass : sha1pass.cc
	$(CXX) $(CPPFLAGS) -o sha1pass sha1pass.cc $(LIBFLAGS)

sha224pass : sha224pass.cc
	$(CXX) $(CPPFLAGS) -o sha224pass sha224pass.cc $(LIBFLAGS)

sha256pass : sha256pass.cc
	$(CXX) $(CPPFLAGS) -o sha256pass sha256pass.cc $(LIBFLAGS)

sha384pass : sha384pass.cc
	$(CXX) $(CPPFLAGS) -o sha384pass sha384pass.cc $(LIBFLAGS)

sha512pass : sha512pass.cc
	$(CXX) $(CPPFLAGS) -o sha512pass sha512pass.cc $(LIBFLAGS)

xxhpass : xxhpass.sh
	cp xxhpass.sh xxhpass

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
	rm -f sumpass
	rm -f md5pass
	rm -f sha1pass
	rm -f sha224pass
	rm -f sha256pass
	rm -f sha384pass
	rm -f sha512pass
	rm -f xxhpass

-include *.d
