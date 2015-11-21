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
LIBFLAGS := -lcrypto -lssl -pthread

all : sump sump-thread

sump : sump.cc
	$(CXX) $(CPPFLAGS) -o sump sump.cc $(LIBFLAGS)

sump-thread : sump-thread.cc
	$(CXX) $(CPPFLAGS) -o sump-thread sump-thread.cc $(LIBFLAGS)

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
	rm -f sump-thread

-include *.d
