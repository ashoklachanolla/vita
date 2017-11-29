#include <stdio.h>
#include <string.h>

#include <iwlib.h>  //!Header for IWLIB interface

#include "WIFIsearch.h"
#include "helper.h"
/*************************************************************************************************************************
 **************************************************************************************************************************
 * NAME                  :   DM_Wifi_Scan(DM_WIFI_ScanResult_Info_t *resultData)
 *
 * DESCRIPTION           :   Linux Wifi Scan API which triggers the iwlib scan api and gets the result
 *
 * INPUTS                :
 *                           INPUT PARAMETERS:
 *
 *                                           NAME :   	TYPE :
 *                                           DESC :
 *
 *                                           NAME :   	TYPE :
 *                                           DESC :
 *
 *                           OUTPUT PARAMETERS:
 *
 *                                           NAME :  resultData 	TYPE : DM_WIFI_ScanResult_Info_t
 *                                           DESC :  returns wifi scan result data with, properties i.e name, mac addr, ssid etc
 **************************************************************************************************************************
 **************************************************************************************************************************/
int DM_Wifi_Scan(DM_WIFI_ScanResult_Info_t *resultData) {
	wireless_scan_head head;
	wireless_scan *result;
	iwrange range;

    unsigned char *mac = NULL;
    int index = 0;	int sock;

	/* Open socket to kernel */
	sock = iw_sockets_open();
    if(sock <= 0)
        return -1;

    //if(sock > 0)    //Check whether WIFI adapter or inbuilt is available
    //{
    /* Get some metadata to use for scanning */
    if (iw_get_range_info(sock, "wlan0", &range) < 0) {
        LOG_PRINT("Error during iw_get_range_info. Aborting.\n");
        //exit(2);
        return -1;
    }

    /* Perform the scan */
    if (iw_scan(sock, "wlan0", range.we_version_compiled, &head) < 0) {
        LOG_PRINT("Error during iw_scan. Aborting.\n");
        //exit(2);
        return -1;
    }

    /* Traverse the results */
    result = head.result;
    while (NULL != result) {
        if( (result->b.essid) == NULL )
            memcpy(resultData[index].wifi_device_names, "[unknown]", strlen("[unknown]"));
        else
            memcpy(resultData[index].wifi_device_names, result->b.essid, strlen(result->b.essid));

        resultData[index].strength = (int8_t)result->stats.qual.level; //!typecasting the value to signed int, so that the result is in dBm
        mac = (unsigned char *)result->ap_addr.sa_data;
        snprintf(resultData[index].macAddr, 30, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
		sprintf(resultData[index].frequency, "%0.3f", (result->b.freq)/1000000000); //Truncating of float value and limit 3 digits
        //!Debug the search results
        /*LOG_PRINT("[%d]Name: %s\n", index, resultData[index].wifi_device_names);
        LOG_PRINT("[%d]Strength: %d\n", index, resultData[index].strength);
        LOG_PRINT("[%d]MAC: %s\n", index, resultData[index].macAddr);
        LOG_PRINT("[%d]Frequency: %s\n", index, resultData[index].frequency);
        //display mac address
        //LOG_PRINT("Mac : %.2X:%.2X:%.2X:%.2X:%.2X:%.2X\n" , mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);*/

        index++;

        result = result->next;
    }

    (resultData+0)->num_elements = index;

    return index;
}
