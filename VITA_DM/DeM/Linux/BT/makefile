#/*
#*	Makefile for the Bluetooth search project			*
#*	Target builds as library and an test application	*
#*/


curr_dir = $(shell pwd)
out_dir = $(curr_dir)/../../../Executables/lib/
INCLUDES = $(curr_dir)/../../../

.PHONY = all
all: libbtsearch.so

libbtsearch.so: BTsearch.o
	@echo "============="
	@echo "VITA_DeM/Source/BT"
	@echo "============="
	@echo "BT"
	$(CC) -shared -o $(out_dir)libbtsearch.so BTsearch.o -lbluetooth
	
BTsearch.o: BTsearch.c
	@echo "Calling BT object generation" 
	$(CC) -c $(CFLAGS) -fPIC -I$(INCLUDES)Utilities -I$(INCLUDES)Headers/ BTsearch.c -o BTsearch.o
	
.PHONY = clean
clean:
	rm -f *.o
