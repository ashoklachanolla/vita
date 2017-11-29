# the compiler: gcc for C program
CC = gcc

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g

curr_dir = $(shell pwd)

DMdir = $(shell pwd)/../Source
libdir = $(curr_dir)/../Executables/lib
INCLUDES = $(curr_dir)/

all: clean build-lib testapp

build-lib: libDM.so

testapp: testapp.o
	@echo "============="
	@echo "VITA_DM/Source"
	@echo "============="
	$(CC) $(CFLAGS) -o testapp testapp.o -L/usr/lib -lm -liw -L$(curr_dir)/../Executables/lib -lDM -lDSM -lDeM -lbtsearch -lwifisearch -lblesearch

testapp.o:
	$(CC) -c $(CFLAGS) -I$(INCLUDES)../Utilities -I$(INCLUDES)../Headers -I$(INCLUDES)../DSM/Headers -o testapp.o main.c

libDM.so:
	@echo "============="
	@echo "VITA_DM/Test - Calling DM Makefile"
	@echo "============="
	$(MAKE) -C $(DMdir) CC=$(CC)

.PHONY = clean
clean:	
	rm -f $(libdir)/*.so
	rm -f ./testapp	
	rm -f *.o
	$(MAKE) -C $(DMdir) clean
	
