#include <stdio.h>
#include <stdlib.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

#include "BTsearch.h"
#include "helper.h"
//!TODO: Implement the Class details of BT
static char *major_classes[] = {"Miscellaneous",
                                "Computer",
                                "Phone",
                                "LAN Access",
                                "Audio/Video",
                                "Peripheral",
                                "Imaging",
                                "Uncategorized"};

int bt_rsp_devices = 0;

/*************************************************************************************************************************
 **************************************************************************************************************************
 * NAME                  :   DM_Wifi_Scan(DM_WIFI_ScanResult_Info_t *resultData)
 *
 * DESCRIPTION           :   Linux Bluetooth Scan API which triggers the hci scan api and gets the result
 *
 * INPUTS                :
 *                           INPUT PARAMETERS:
 *
 *                                           NAME :   	TYPE :
 *                                           DESC :
 *
 *                           OUTPUT PARAMETERS:
 *
 *                                           NAME :  resultData 	TYPE : DM_WIFI_ScanResult_Info_t
 *                                           DESC :  returns bluetooth scan result data with, properties i.e name, mac addr etc
 *                                          
 *                                           NAME : result  	TYPE : int
 *                                           DESC : buetooth scan results
 **************************************************************************************************************************
 **************************************************************************************************************************/

int DM_BT_Scan(DM_BT_ScanResult_Info_t *resultData)
{
    inquiry_info *ii = NULL;
	int max_rsp;
	int dev_id, sock, len, flags;
	int i;
	char addr[19] = {0};
	char name[248] = {0};

	dev_id = hci_get_route(NULL);
	sock = hci_open_dev( dev_id );
	if (dev_id < 0 || sock < 0) {        
        perror("opening socket");
        return -1;
	}

	len = 8;
	max_rsp = 255;
	flags = IREQ_CACHE_FLUSH;
	ii = (inquiry_info*)malloc(max_rsp * sizeof(inquiry_info));
	
    bt_rsp_devices = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);
    if( bt_rsp_devices < 0 ) {
		perror("hci_inquiry");
        free(ii);
        close(sock);
        return -1;
	}
    else if(bt_rsp_devices == 0) {
		LOG_PRINT("No devices found\n");
        return 0;
	}

    for (i = 0; i < bt_rsp_devices; i++) {
		ba2str(&(ii+i)->bdaddr, addr);
		memset(name, 0, sizeof(name));
        if (hci_read_remote_name(sock, &(ii+i)->bdaddr, sizeof(name), name, 0) < 0)
            strcpy(name, "[unknown]");

        memcpy(resultData[i].bt_name, name, strlen(name));
        memcpy(resultData[i].bt_addr, addr, strlen(addr));
        resultData[i].bt_class = (*(ii+i)->dev_class+0);

        //!Debug the search results
        //printf("[RAW: %d]%s  %s %d\n", i, name, addr, (*(ii+i)->dev_class+0));
        //printf("[RESULTDATA]%s  %s %d\n", resultData[i].bt_name, resultData[i].bt_addr, resultData[i].bt_class);
    }
    
    resultData[0].num_elements = bt_rsp_devices;

    free(ii);
    close(sock);

    return resultData[0].num_elements;
}
