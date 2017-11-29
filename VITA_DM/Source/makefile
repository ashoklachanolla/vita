#/*
#*	Makefile for the Data Manager project				*
#*	Target builds as library and an test application	*
#*/
CC = gcc

curr_dir = $(shell pwd)/
out_dir = $(curr_dir)../Executables/lib/
DeMdir = $(curr_dir)../DeM/Source
DsMdir = $(curr_dir)../DSM/Source
INCLUDES = $(curr_dir)

.PHONY = all

VPATH = ../Utilities:json

all: libDM.so libDeM.so libDSM.so

libDM.so: VITA_DM.o VITA_DM_BT.o VITA_DM_WIFI.o VITA_DM_BLE.o helper.o json_helper.o
	$(CC) -shared -o $(out_dir)libDM.so VITA_DM.o VITA_DM_BT.o VITA_DM_WIFI.o VITA_DM_BLE.o $(curr_dir)../Utilities/helper.o json/json_helper.o -ljson-c -lpthread
	
VITA_DM.o: VITA_DM.c
	@echo "============="
	@echo "VITA_DM/Source"
	@echo "============="
	$(CC) -c $(CFLAGS) -fPIC -I$(INCLUDES)../Utilities -I$(INCLUDES)../Headers -I$(INCLUDES)json -I$(INCLUDES)../DeM/Headers -I$(INCLUDES)../DSM/Headers -I$(curr_dir)../Utilities/ -I$(INCLUDES)../DM_IoTvity/ VITA_DM.c -o VITA_DM.o

VITA_DM_BT.o: VITA_DM_BT.c
	$(CC) -c $(CFLAGS) -fPIC -I$(INCLUDES)../Utilities -I$(INCLUDES)../Headers -I$(INCLUDES)../DeM/Headers -I$(INCLUDES)../DSM/Headers VITA_DM_BT.c -o VITA_DM_BT.o

VITA_DM_WIFI.o: VITA_DM_WIFI.c
	$(CC) -c $(CFLAGS) -fPIC -I$(INCLUDES)../Utilities -I$(INCLUDES)../Headers -I$(INCLUDES)../DeM/Headers -I$(INCLUDES)../DSM/Headers VITA_DM_WIFI.c -o VITA_DM_WIFI.o

VITA_DM_BLE.o: VITA_DM_BLE.c
	$(CC) -c $(CFLAGS) -fPIC -I$(INCLUDES)../Utilities -I$(INCLUDES)../Headers -I$(INCLUDES)../DeM/Headers -I$(INCLUDES)../DSM/Headers VITA_DM_BLE.c -o VITA_DM_BLE.o

json_helper.o: json_helper.c
	$(CC) -c $(CFLAGS) -fPIC -I$(INCLUDES)../Utilities -I$(INCLUDES)../Headers -I$(INCLUDES)../Utilities -I$(INCLUDES)../Test -I$(INCLUDES)json json/json_helper.c -o json/json_helper.o
	
helper.o: helper.c
	$(CC) -c $(CFLAGS) -fPIC -I$(INCLUDES)../Headers -I$(curr_dir)../Source/json $(curr_dir)../Utilities/helper.c -o $(curr_dir)../Utilities/helper.o

libDeM.so:
	@echo "============="
	@echo "VITA_DM/Source - Calling DeM Makefile"
	@echo "============="
	$(MAKE) -C $(DeMdir) CC=$(CC) CFLAGS=$(CFLAGS)
	
libDSM.so:
	@echo "============="
	@echo "VITA_DM/Source - Calling DSM Makefile"
	@echo "============="
	$(MAKE) -C $(DsMdir) CC=$(CC) CFLAGS=$(CFLAGS)
	
.PHONY = clean
clean:
	$(MAKE) -C $(DeMdir) clean
	$(MAKE) -C $(DsMdir) clean
	rm -f *.o $(curr_dir)json/*.o $(curr_dir)../Utilities/*.o
