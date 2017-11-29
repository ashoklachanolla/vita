
curr_dir = $(shell pwd)
out_dir = $(curr_dir)/../../Executables/lib/
INCLUDES = $(curr_dir)/../../

libDSM.so: VITA_DM_DSM.o curl_operation.o VITA_DM_BT_DSM.o VITA_DM_WIFI_DSM.o VITA_DM_BLE_DSM.o VITA_DM_IoTvity_DSM.o
	$(CC) -shared -o $(out_dir)libDSM.so VITA_DM_DSM.o VITA_DM_WIFI_DSM.o VITA_DM_BT_DSM.o VITA_DM_BLE_DSM.o VITA_DM_IoTvity_DSM.o curl_operation.o -lcurl -ljson-c 

VITA_DM_DSM.o: VITA_DM_DSM.c 
	$(CC) -c $(CFLAGS) -fPIC -I$(INCLUDES)Utilities -I$(INCLUDES)Headers/ -I$(INCLUDES)DSM/Headers/ VITA_DM_DSM.c -o VITA_DM_DSM.o

curl_operation.o: curl_operation.c
	$(CC) -c $(CFLAGS) -fPIC -I$(INCLUDES)Utilities -I$(INCLUDES)Headers/ -I$(INCLUDES)DSM/Headers/ curl_operation.c -o curl_operation.o

VITA_DM_BT_DSM.o: VITA_DM_BT_DSM.c
	$(CC) -c $(CFLAGS) -fPIC -I$(INCLUDES)Utilities -I$(INCLUDES)Headers/ -I$(INCLUDES)DSM/Headers/ VITA_DM_BT_DSM.c -o VITA_DM_BT_DSM.o
	
VITA_DM_WIFI_DSM.o: VITA_DM_WIFI_DSM.c
	$(CC) -c $(CFLAGS) -fPIC -I$(INCLUDES)Utilities -I$(INCLUDES)Headers/ -I$(INCLUDES)DSM/Headers/ VITA_DM_WIFI_DSM.c -o VITA_DM_WIFI_DSM.o
	
VITA_DM_BLE_DSM.o: VITA_DM_BLE_DSM.c
	$(CC) -c $(CFLAGS) -fPIC -I$(INCLUDES)Utilities -I$(INCLUDES)Headers/ -I$(INCLUDES)DSM/Headers/ VITA_DM_BLE_DSM.c -o VITA_DM_BLE_DSM.o

VITA_DM_IoTvity_DSM.o: VITA_DM_IoTvity_DSM.c
	$(CC) -c $(CFLAGS) -fPIC -I$(INCLUDES)Utilities -I$(INCLUDES)Headers/ -I$(INCLUDES)DSM/Headers/ VITA_DM_IoTvity_DSM.c -o VITA_DM_IoTvity_DSM.o

	
.PHONY = clean
clean:
	rm -f *.o
