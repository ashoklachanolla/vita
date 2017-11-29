/**********************************************************************************************
* VOTARYTECH SOFTTECH SOLUTIONS PVT. LTD. CONFIDENTIAL [2009]-[2016]
*  All Rights Reserved. *
* NOTICE: This file is part of {project}.
* All information contained herein is, and remains the property of Votary Softech Solutions Pvt. Ltd.
* and its suppliers,if any.
* Dissemination of this information or reproduction of this material  is strictly forbidden unless prior written permission is obtained
* from Votary Softech Solutions Pvt. Ltd.
.
Author:                                                Date:
Description:
 
Revision History
----------------------
Bug ID:                                             Author:                                                                Date:
ChangeID:                                           Author:                                                                Date:
 
************************************************************************************************/
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "VITA_DM.h"
#include "VITA_DM_INTERNAL.h"
#include "json_helper.h"
#include "helper.h"
#include <time.h>
#include <stdlib.h>
#include "DM_IoTvity_api.h"


//! URL Logic variables
char server_url[255] = {""};
char device_ID[255] = {""};
bool has_url = false;
bool has_device_ID = false;

char wifiScantimeStamp[35];
char btScantimeStamp[35];
char bleScantimeStamp[35];


/*************************************************************************************************************************
**************************************************************************************************************************
* NAME                  :   VITA_DM_Init(DM_Init_Info_t Init_info, char url[])
*
* DESCRIPTION           :   VITA_DM_Init api is used to pass init info i.e received Wifi & Bt params for CM, DSM, DEM, deviceID, sm_url from TL to DM layer
*
* INPUTS                :
*                           INPUT PARAMETERS:
*
*                                           NAME : Init_info 	TYPE : DM_Init_Info_t
*                                           DESC  : To get init info data
*
*                           OUTPUT PARAMETERS:
*
*                                           NAME : int 	TYPE : int
*                                           DESC  : to return a value
*
**************************************************************************************************************************
**************************************************************************************************************************/

int VITA_DM_Init(DM_Init_Info_t Init_info, char url[])
{

    DM_IoTVity_Init();

    if(VITA_DM_BT_Init() == 0)
        VITA_DM_INIT_PostResponce(VITA_DM_INIT_SUCESS);
    else
        VITA_DM_INIT_PostResponce(VITA_DM_INIT_FAIL);

    if(VITA_DM_WIFI_Init() == 0)
        VITA_DM_INIT_PostResponce(VITA_DM_INIT_SUCESS);
    else
        VITA_DM_INIT_PostResponce(VITA_DM_INIT_FAIL);

    if(VITA_DM_BLE_Init() == 0)
        VITA_DM_INIT_PostResponce(VITA_DM_INIT_SUCESS);
    else
        VITA_DM_INIT_PostResponce(VITA_DM_INIT_FAIL);

    if(strlen(url))
    {
        has_url = true;
        strncpy(server_url, url, strlen(url));
        //printf("<%s:%d> len: %d\t URL:%s\n", __func__, __LINE__, (int)strlen(server_url), server_url);
        LOG_PRINT("len: %d\t URL:%s",(int)strlen(server_url), server_url);
    }
    else
    {
        //printf("No URL provided");
        LOG_PRINT("No URL provided");
    }
    if(Init_info.DeviceId)
    {
        has_device_ID = true;
        strncpy(device_ID, Init_info.DeviceId, strlen(Init_info.DeviceId));
        //printf("<%s:%d> len: %d\t Dev ID:%s\n", __func__, __LINE__, (int)strlen(Init_info.DeviceId), device_ID);
        LOG_PRINT("len: %d\t Dev ID:%s",(int)strlen(Init_info.DeviceId), device_ID);
    }

    return 0;
}
int VITA_DM_Init_ext(DM_Init_Info_t Init_info, char url[])
{
//    if(VITA_DM_BT_Init() == 0)
//        VITA_DM_INIT_PostResponce(VITA_DM_INIT_SUCESS);
//    else
//        VITA_DM_INIT_PostResponce(VITA_DM_INIT_FAIL);

//    if(VITA_DM_WIFI_Init() == 0)
//        VITA_DM_INIT_PostResponce(VITA_DM_INIT_SUCESS);
//    else
//        VITA_DM_INIT_PostResponce(VITA_DM_INIT_FAIL);

//    if(VITA_DM_BLE_Init() == 0)
//        VITA_DM_INIT_PostResponce(VITA_DM_INIT_SUCESS);
//    else
//        VITA_DM_INIT_PostResponce(VITA_DM_INIT_FAIL);

    DM_IoTVity_Init();
    if(strlen(url))
    {
        has_url = true;
        strncpy(server_url, url, strlen(url));
        //printf("<%s:%d> len: %d\t URL:%s\n", __func__, __LINE__, (int)strlen(server_url), server_url);
        LOG_PRINT("len: %d\t URL:%s",(int)strlen(server_url), server_url);
    }
    else
    {
        //printf("No URL provided");
        LOG_PRINT("No URL provided");
    }
    if(Init_info.DeviceId)
    {
        has_device_ID = true;
        strncpy(device_ID, Init_info.DeviceId, strlen(Init_info.DeviceId));
        //printf("<%s:%d> len: %d\t Dev ID:%s\n", __func__, __LINE__, (int)strlen(Init_info.DeviceId), device_ID);
        LOG_PRINT("len: %d\t Dev ID:%s",(int)strlen(Init_info.DeviceId), device_ID);
    }

    return 0;
}

/*************************************************************************************************************************
 **************************************************************************************************************************
 * NAME                  :   VITA_DM_cleanup()
 *
 * DESCRIPTION           :   to free(memory) server_url value
 *
 * INPUTS                :
 *                           INPUT PARAMETERS:
 *
 *                                           NAME : nil 	TYPE :
 *                                           DESC  :
 *
 *                           OUTPUT PARAMETERS:
 *
 *                                           NAME : nil 	TYPE :
 *                                           DESC  :
 *
 **************************************************************************************************************************
 **************************************************************************************************************************/

void VITA_DM_cleanup()
{

}

/*************************************************************************************************************************
 **************************************************************************************************************************
 * NAME                  :   DM_GetSM_URL(char **url)
 *
 * DESCRIPTION           :   To Get server_url
 *
 * INPUTS                :
 *                           INPUT PARAMETERS:
 *
 *                                           NAME : nil  	TYPE :
 *                                           DESC  :
 *
 *                           OUTPUT PARAMETERS:
 *
 *                                           NAME : url 	TYPE : char
 *                                           DESC  : url parameter is returned with server url
 *
 *                                           NAME : int 	TYPE : int
 *                                           DESC  : to return a value
 **************************************************************************************************************************
 **************************************************************************************************************************/

int DM_GetSM_URL(char **url)
{
    if(has_url)
    {
        *url = server_url;
        //printf("<%s:%d> len: %d\t URL:%s\n", __func__, __LINE__, (int)strlen(server_url), server_url);
        LOG_PRINT("len: %d\t URL:%s", (int)strlen(server_url), server_url);
        return strlen(server_url);
    }
    return -1;
}

/*************************************************************************************************************************
 **************************************************************************************************************************
 * NAME                  :   DM_Get_Device_ID(char **deviceid)
 *
 * DESCRIPTION           :   To Get device_id
 *
 * INPUTS                :
 *                           INPUT PARAMETERS:
 *
 *                                           NAME : nil  	TYPE :
 *                                           DESC  :
 *
 *                           OUTPUT PARAMETERS:
 *
 *                                           NAME : deviceid 	TYPE : char
 *                                           DESC  : deviceid parameter is returned with  deviceid
 *
 *                                           NAME : int 	TYPE : int
 *                                           DESC  : to return a value
 **************************************************************************************************************************
 **************************************************************************************************************************/

int DM_Get_Device_ID(char **deviceid)
{
    if(has_device_ID)
    {
        *deviceid = device_ID;
        //printf("<%s:%d> len: %d\t Dev ID:%s\n", __func__, __LINE__, (int)strlen(device_ID), device_ID);
       // LOG_PRINT("len: %d\t Dev ID:%s", (int)strlen(device_ID), device_ID);
        return strlen(device_ID);
    }
    return -1;
}


/*************************************************************************************************************************
 **************************************************************************************************************************
 * NAME                  :   VITA_DM_Open(DM_Open_Info_t Open_info)
 *
 * DESCRIPTION           :
 *
 * INPUTS                :
 *                           INPUT PARAMETERS:
 *
 *                                           NAME : Open_info  	TYPE : DM_Open_Info_t
 *                                           DESC  :
 *
 *                           OUTPUT PARAMETERS:
 *
 *                                           NAME : int 	TYPE : int
 *                                           DESC  : to return a value
 **************************************************************************************************************************
 **************************************************************************************************************************/

int VITA_DM_Open(DM_Open_Info_t Open_info)
{
    static int Handle=1;
	return Handle++;
}

/*************************************************************************************************************************
 **************************************************************************************************************************
 * NAME                  :   VITA_DM_Scan(int Handle, VITA_DM_ScanType_e scan_info)
 *
 * DESCRIPTION           :
 *
 * INPUTS                :
 *                           INPUT PARAMETERS:
 *
 *                                           NAME : scan_info  	TYPE : VITA_DM_ScanType_e
 *                                           DESC : to pass scan info data
 *
 *                                           NAME : Handle  	TYPE : int
 *                                           DESC :
 *
 *                           OUTPUT PARAMETERS:
 *
 *                                           NAME : int 	TYPE : int
 *                                           DESC : to return a value
 **************************************************************************************************************************
 **************************************************************************************************************************/
int VITA_DM_Scan(int Handle, DM_Scan_cmd_data scanData)
{    

    struct timeval tv;

    gettimeofday(&tv, NULL);

    unsigned long long millisecondsSinceEpoch =
        (unsigned long long)(tv.tv_sec) * 1000 +
        (unsigned long long)(tv.tv_usec) / 1000;

   // printf("VITA_DM_Scan === > %llu\n", millisecondsSinceEpoch);


    char timeStamp[35];
        sprintf(timeStamp, "%llu", millisecondsSinceEpoch);


    if(scanData.deviceType == DEVICE_BLUETOOTH)
    {
        if(!VITA_DM_BT_Scan(scanData.scanType))
            strcpy(btScantimeStamp,timeStamp);

    }
    else if(scanData.deviceType == DEVICE_WIFI)
    {
        if(!VITA_DM_WIFI_Scan(scanData.scanType))
            strcpy(wifiScantimeStamp,timeStamp);

    }
    else if (scanData.deviceType == DEVICE_BLE)
    {
        if(!VITA_DM_BLE_Scan(scanData.scanType))
            strcpy(bleScantimeStamp,timeStamp);

    }

    else if(scanData.deviceType == DEVICE_ALL)
    {
//        VITA_DM_BT_Scan(scanData.scanType);
//        VITA_DM_WIFI_Scan(scanData.scanType);
//        VITA_DM_BLE_Scan(scanData.scanType);


        if(!VITA_DM_BT_Scan(scanData.scanType))
            strcpy(btScantimeStamp,timeStamp);
        if(!VITA_DM_WIFI_Scan(scanData.scanType))
            strcpy(wifiScantimeStamp,timeStamp);
//        if(!VITA_DM_BLE_Scan(scanData.scanType))
//            strcpy(bleScantimeStamp,timeStamp);


    }
    printf("BTTimestamp: %s\nBLETimeStamp: %s\nWIfiTimeStamp: %s\n\n",btScantimeStamp,bleScantimeStamp,wifiScantimeStamp);

    return 0;
}

int VITA_DM_Discover()
{
    struct timeval tv;

    gettimeofday(&tv, NULL);

    unsigned long long millisecondsSinceEpoch =
        (unsigned long long)(tv.tv_sec) * 1000 +
        (unsigned long long)(tv.tv_usec) / 1000;

   // printf("VITA_DM_Scan === > %llu\n", millisecondsSinceEpoch);


    char timeStamp[35];
        sprintf(timeStamp, "%llu", millisecondsSinceEpoch);

    strcpy(wifiScantimeStamp,timeStamp);

    DM_IoTVity_Discovery();
    //printf("Discover called\n");
}

void VITA_DM_IoTvity_Get(const char *handle)
{
    printf("inside get\n");
    DM_IoTVity_Get(handle);
}
void VITA_DM_IoTvity_Put(const char *handle,json_object *obj)
{
    DM_IoTVity_Put(handle,obj);

}
void VITA_DM_IoTvity_Observe(const char *handle)
{
    DM_IoTVity_Observe(handle);
}


void VITA_DM_Abort(DM_Scan_cmd_data scanData)
{
    if(scanData.deviceType == DEVICE_BLUETOOTH)
    {
        VITA_DM_BT_Abort();
    }
    else if(scanData.deviceType == DEVICE_WIFI)
    {
        VITA_DM_WIFI_Abort();
    }
    else if(scanData.deviceType == DEVICE_BLE)
    {
        VITA_DM_BLE_Abort();
    }
    else if(scanData.deviceType == DEVICE_ALL)
    {
        VITA_DM_BT_Abort();
        VITA_DM_WIFI_Abort();
    }
}

/*************************************************************************************************************************
 **************************************************************************************************************************
 * NAME                  :   VITA_DM_BT_PostResponce(VITA_SCAN_Type scanType ,DM_BT_SCAN_RESPONCE_e bt_response)
 *
 * DESCRIPTION           :   To Send BT Scan Response to Smart App
 *
 * INPUTS                :
 *                           INPUT PARAMETERS:
 *
 *                                           NAME : scanType  	TYPE : VITA_DM_ScanType
 *                                           DESC : To indicate AUTO/MANUAL
 *
 *                                           NAME : bt_response  	TYPE : DM_BT_SCAN_RESPONCE_e
 *                                           DESC : enum for BT scan status i.e success/failure
 *
 *                           OUTPUT PARAMETERS:
 *
 *                                           NAME :  	TYPE :
 *                                           DESC :
 **************************************************************************************************************************
 **************************************************************************************************************************/

void VITA_DM_BT_PostResponce(VITA_SCAN_Type scanType ,DM_BT_SCAN_RESPONCE_e bt_response)
{
    VITA_DM_create_BT_response(scanType,bt_response);
}

void VITA_DM_BLE_PostResponce(VITA_SCAN_Type scanType ,DM_BLE_SCAN_RESPONCE_e ble_response)
{
    VITA_DM_create_BLE_response(scanType,ble_response);
}

/*************************************************************************************************************************
 **************************************************************************************************************************
 * NAME                  :   VITA_DM_WiFi_PostResponce(VITA_SCAN_Type scanType ,DM_WIFI_SCAN_RESPONCE_e wifi_response)
 *
 * DESCRIPTION           :   To Send WiFi Scan Response to Smart App
 *
 * INPUTS                :
 *                           INPUT PARAMETERS:
 *
 *                                           NAME : scanType  	TYPE : VITA_SCAN_Type
 *                                           DESC : To indicate AUTO/MANUAL
 *
 *                                           NAME : bt_response  	TYPE : DM_WIFI_SCAN_RESPONCE_e
 *                                           DESC : enum for WiFi scan status i.e success/failure
 *
 *                           OUTPUT PARAMETERS:
 *
 *                                           NAME :  	TYPE :
 *                                           DESC :
 **************************************************************************************************************************
 **************************************************************************************************************************/

void VITA_DM_WiFi_PostResponce(VITA_SCAN_Type scanType ,DM_WIFI_SCAN_RESPONCE_e wifi_response)
{
    VITA_DM_create_WIFI_Response(scanType,wifi_response);
}

/*************************************************************************************************************************
 **************************************************************************************************************************
 * NAME                  :   VITA_DM_INIT_PostResponce (DM_INIT_RESPONCE_e init_response)
 *
 * DESCRIPTION           :   To Send INIT Response to Smart App
 *
 * INPUTS                :
 *                           INPUT PARAMETERS:
 *
 *                                           NAME : init_response  	TYPE : DM_INIT_RESPONCE_e
 *                                           DESC : To indicate AUTO/MANUAL
 *
 *                                           NAME : bt_response  	TYPE : DM_WIFI_SCAN_RESPONCE_e
 *                                           DESC : enum for init status response i.e success/failure
 *
 *                           OUTPUT PARAMETERS:
 *
 *                                           NAME :  	TYPE :
 *                                           DESC :
 **************************************************************************************************************************
 **************************************************************************************************************************/


void VITA_DM_INIT_PostResponce(DM_INIT_RESPONCE_e init_response)
{
    VITA_DM_create_Init_response(init_response);
}

char *VITA_DM_getTimeStamp(VITA_DM_DEVICETYPE_e scanType)
{
    char timeStamp[35];
    if(scanType == DEVICE_BLUETOOTH)
    {
        return btScantimeStamp;
    }
    else if(scanType == DEVICE_WIFI)
    {
        return wifiScantimeStamp;
    }
    else if(scanType == DEVICE_BLE)
    {
        return bleScantimeStamp;
    }

    return timeStamp;
}
