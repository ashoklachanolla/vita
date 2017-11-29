1.in the function "CALEClientSendDataThread" in caleadapter.c
add the following code at the start
while(check_for_ble_status_flag()==false)
    {
        sleep(2);
    }


2. In the file central.c
	add the following prototype
		extern int CONNECT_IOT_DM(char *);
	add the following code in the function "CACentralConnect" after checking connection status in "if" block and comment the remaining code.
		 char * address =g_variant_print(g_dbus_proxy_get_cached_property(device,"Address"),TRUE);
		address[18]='\0';
		return CONNECT_IOT_DM(address+1);
3. In the file client.c
	add the following code as global
		typedef struct BLE_FILE_DESCRIPTOR
			{
			    int fd;
			    char address[18];
			}BLE_DEVICES;

			extern int count_ble;
			extern BLE_DEVICES *ble_pointer;
			extern void VITA_WRITE_REQUEST(int fd,uint8_t * data,uint8_t length);
	add the following code in the function "CAGattClientSendDataImpl" and comment everything
		    unsigned char wpdu[512];
		    wpdu[0] = 0x12;// BT_ATT_OP_WRITE_REQ;
			wpdu[1]=0x0c;
			wpdu[2]=0x00;
		    memcpy(wpdu+3,data,length);
		    for(int i=0;i<count_ble;i++)
		    {
			printf("in for loop  fd = %d\n\n",ble_pointer[i].fd);

			   VITA_WRITE_REQUEST(ble_pointer[i].fd,wpdu,length+3);

		  	  }
	add the following code in the function "CAGattClientSendData" and comment everything
		 assert(context != NULL);
		    unsigned char wpdu[512];
		    wpdu[0] =0x12;// BT_ATT_OP_WRITE_REQ;
			wpdu[1]=0x0c;
			wpdu[2]=0x00;
		    memcpy(wpdu+3,data,length);
		    for(int i=0;i<count_ble;i++)
		    {
		       if(strcmp(address,ble_pointer[i].address)==0)
			{
			   printf("\nfd==%d\n",ble_pointer[i].fd);
			   VITA_WRITE_REQUEST(ble_pointer[i].fd,wpdu,length+3);
			   break;
			}
		    		}
	add the following code in the function "CAGattClientSendDataToAll" and comment everything

		    assert(context != NULL);
		    CAResult_t result = CA_STATUS_FAILED;
			result = CAGattClientSendDataImpl(NULL,
				                           data,
				                         length,
				                           context);

		    return result;
4. Include the "IOTivity.c" file in the caleadapter.c file after all prototype declarations


