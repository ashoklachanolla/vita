curr_dir = $(shell pwd)
out_dir = $(curr_dir)/../../Executables/lib/
wifidir = $(shell pwd)/../Linux/WIFI
btdir = $(shell pwd)/../Linux/BT
bledir = $(shell pwd)/../Linux/BLE
INCLUDES = $(curr_dir)/../../

.PHONY = all
all: libDeM.so libbtsearch.so libwifisearch.so libblesearch.so

libDeM.so: DeMWiFi.o DeMBT.o DeMBLE.o
	$(CC) $(CFLAGS) -shared -o $(out_dir)libDeM.so DeM_BT.o DeM_WiFi.o DeM_BLE.o

DeMWiFi.o: DeM_WiFi.c 
	$(CC) -c $(CFLAGS) -fPIC -I$(INCLUDES)Utilities -I$(INCLUDES)Headers/ -I$(INCLUDES)DeM/Headers/ -I$(INCLUDES)DeM/Linux/WIFI/ DeM_WiFi.c -o DeM_WiFi.o -L$(out_dir) -lwifisearch

DeMBT.o: DeM_BT.c 
	$(CC) -c $(CFLAGS) -fPIC -I$(INCLUDES)Utilities -I$(INCLUDES)Headers/ -I$(INCLUDES)DeM/Headers/ -I$(INCLUDES)DeM/Linux/BT/ DeM_BT.c -o DeM_BT.o -L$(out_dir) -lbtsearch

DeMBLE.o: DeM_BLE.c
	$(CC) -c $(CFLAGS) -fPIC -I$(INCLUDES)Utilities -I$(INCLUDES)Headers/ -I$(INCLUDES)/DeM/Headers -I$(INCLUDES)DeM/Linux/BLE/ DeM_BLE.c -o DeM_BLE.o -L$(out_dir) -lblesearch

#LINUX
libbtsearch.so:
	@echo "============="
	@echo "VITA_DeM/Source - Calling BT"
	@echo "============="
	$(MAKE) -C $(btdir) CC=$(CC)

libwifisearch.so:
	@echo "============="
	@echo "VITA_DeM/Source - Calling WIFI"
	@echo "============="
	$(MAKE) -C $(wifidir) CC=$(CC)
	
libblesearch.so:
	@echo "============="
	@echo "VITA_DeM/Source - Calling BLE"
	@echo "============="
	$(MAKE) -C $(bledir) CC=$(CC)

#Android

#iOS

.PHONY = clean
clean:
	$(MAKE) -C $(btdir) clean
	$(MAKE) -C $(wifidir) clean
	$(MAKE) -C $(bledir) clean
	rm -f *.o
