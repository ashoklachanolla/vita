#/*
#*	Makefile for the Bluetooth search project			*
#*	Target builds as library and an test application	*
#*/


curr_dir = $(shell pwd)
out_dir = $(curr_dir)/../../../Executables/lib/
INCLUDES = $(curr_dir)/../../../

.PHONY = all
all: libblesearch.so

libblesearch.so: BLEsearch.o
	@echo "============="
	@echo "VITA_DeM/Source/BLE"
	@echo "============="
	@echo "BLE"
	$(CC) -shared -o $(out_dir)libblesearch.so BLEsearch.o -lbluetooth
	
BLEsearch.o: BLEsearch.c
	@echo "Calling BLE object generation" 
	$(CC) -c $(CFLAGS) -fPIC -I$(INCLUDES)Utilities -I$(INCLUDES)Headers/ BLEsearch.c -o BLEsearch.o
	
.PHONY = clean
clean:
	rm -f *.o
