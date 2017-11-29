#/*
#*	Makefile for the WIFI search project				*
#*	Target builds as library and an test application	*
#*/

curr_dir = $(shell pwd)
out_dir = $(curr_dir)/../../../Executables/lib/
INCLUDES = $(curr_dir)/../../../

objdir=$(objdir)

.PHONY = all
all: libwifisearch.so

libwifisearch.so: WIFIsearch.o
	@echo "============="
	@echo "VITA_DeM/Source/WIFI"
	@echo "============="
	@echo "WIFI"
	$(CC) -shared -o $(out_dir)libwifisearch.so WIFIsearch.o -L/usr/lib -liw -lm

WIFIsearch.o: WIFIsearch.c
	@echo "Calling WIFI object generation" 	
	$(CC) -c $(CFLAGS) -fPIC -I$(INCLUDES)Utilities -I$(INCLUDES)Headers/ WIFIsearch.c -o WIFIsearch.o

.PHONY = clean
clean:
	rm -f *.o
