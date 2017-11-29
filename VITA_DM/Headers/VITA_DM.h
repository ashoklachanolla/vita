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
#ifndef VITA_DM_H
#define VITA_DM_H

#ifdef __cplusplus
extern "C" {
#endif
#include <json-c/json.h>
#include <curl/curl.h>


//! For Bluetooth
#define BT_Param_Name "Name"
#define BT_Param_DeviceID "DeviceID"
#define BT_Param_Class "Class_Name"

//! For WIFI
#define WIFI_Param_Name "Name"
#define WIFI_Param_DeviceID "DeviceID"
#define WIFI_Param_Signal "Signal"
#define WIFI_Param_Frequency "Frequency"

//! For BLE
#define BLE_Param_Name "Name"
#define BLE_Param_DeviceID "DeviceID"
#define BLE_Param_Class "Class_Name"


#define MAX_SCAN_CMD_SIZE 50
#define DM_DEVICEID_SIZE 50
#define DM_SM_URL_SIZE 255
//#define TEST

typedef enum {
    DEVICE_BLUETOOTH = 1,
    DEVICE_WIFI,
    DEVICE_BLE,
    DEVICE_ALL
    //! Need to add if more devices supported is required
}VITA_DM_DEVICETYPE_e; //!Type of devices supported

typedef enum {
    AUTO = 1,
    MANUAL
}VITA_SCAN_Type; //!Scan Type Auto or Manual

typedef enum {
    VITA_DM_INIT_SUCESS,
    VITA_DM_INIT_FAIL,
}DM_INIT_RESPONCE_e; //! Type of INIT scan status

typedef enum {
    STATE_INIT = 1,
    STATE_READY,
    STATE_SCANNING_AUTO,
    STATE_SCANNING_MANUAL,
    STATE_SCANNING_ABORT
}Scanning_State_e;

//! BT Filter Block
typedef  enum {
    VITA_DM_BT_SCAN_SUCESS,
    VITA_DM_BT_SCAN_FAIL,
    VITA_DM_BT_SCAN_STORE_FAIL,
    VITA_DM_BT_SCAN_NO_DEVICE_FOUND,
    VITA_DM_BT_SCAN_ABORTED,
    VITA_DM_BT_SCAN_PROGRESS
}DM_BT_SCAN_RESPONCE_e; //! Type of BT scan status

typedef struct {

} DM_BT_DeM_Filter_t;

typedef struct {

} DM_BT_CM_Filter_t;

typedef struct {
    int bIncludeBtName;
    int bIncludeBtDeviceID;
    int bIncludeBtClass;
} DM_BT_DSM_Filter_t;
//! BT Filter Block

typedef struct {
    //DM_BT_DeM_Filter_t DM_BT_DeM_Filter;
    //DM_BT_CM_Filter_t  DM_BT_CM_Filter;
    DM_BT_DSM_Filter_t DM_BT_DSM_Filter;
}DM_BT_VITA_Filter_t;

//!WIFI Filter Block
typedef enum {
    VITA_DM_WIFI_SCAN_SUCESS,
    VITA_DM_WIFI_SCAN_FAIL,
    VITA_DM_WIFI_SCAN_STORE_FAIL,
    VITA_DM_WIFI_SCAN_NO_DEVICE_FOUND,
    VITA_DM_WIFI_SCAN_ABORTED,
    VITA_DM_WIFI_SCAN_PROGRESS
}DM_WIFI_SCAN_RESPONCE_e; //! Type of WIFI scan status

typedef struct {

}DM_WIFI_DeM_Filter_t;

typedef struct {

}DM_WIFI_CM_Filter_t;

typedef struct {
    int bIncludeWifiName;
    int bIncludeWifiSignal;
    int bIncludeWifiDeviceID;
    int bIncludeWifiFrequency;
}DM_WIFI_DSM_Filter_t;
//!WIFI Filter Block

typedef struct {
    //DM_WIFI_DeM_Filter_t DM_WIFI_DeM_Filter;
    //DM_WIFI_CM_Filter_t  DM_WIFI_CM_Filter;
    DM_WIFI_DSM_Filter_t DM_WIFI_DSM_Filter;
}DM_WIFI_VITA_Filter_t;



//! BLE Filter Block
typedef  enum {
    VITA_DM_BLE_SCAN_SUCESS,
    VITA_DM_BLE_SCAN_FAIL,
    VITA_DM_BLE_SCAN_STORE_FAIL,
    VITA_DM_BLE_SCAN_NO_DEVICE_FOUND,
    VITA_DM_BLE_SCAN_ABORTED,
    VITA_DM_BLE_SCAN_PROGRESS
}DM_BLE_SCAN_RESPONCE_e; //! Type of BLE scan status

typedef struct {

} DM_BLE_DeM_Filter_t;

typedef struct {

} DM_BLE_CM_Filter_t;

typedef struct {
    int bIncludeBleName;
    int bIncludeBleDeviceID;
    int bIncludeBleClass;
} DM_BLE_DSM_Filter_t;
//! BLE Filter Block

typedef struct {
    //DM_BT_DeM_Filter_t DM_BLE_DeM_Filter;
    //DM_BT_CM_Filter_t  DM_BLE_CM_Filter;
    DM_BLE_DSM_Filter_t DM_BLE_DSM_Filter;
}DM_BLE_VITA_Filter_t;


typedef struct {
    char DeviceId[DM_DEVICEID_SIZE];
    char SM_URL[DM_SM_URL_SIZE];
    DM_BT_VITA_Filter_t BT_VITA_Filter;
    DM_WIFI_VITA_Filter_t WIFI_VITA_Filter;
    DM_BLE_VITA_Filter_t BLE_VITA_Filter;

}DM_Init_Info_t;

typedef struct {
    char cmd[MAX_SCAN_CMD_SIZE];
    VITA_DM_DEVICETYPE_e deviceType;
    VITA_SCAN_Type scanType;
}DM_Scan_cmd_data;

typedef Scanning_State_e Scan_state_t;
typedef char* DM_Open_Info_t;

int VITA_DM_Init(DM_Init_Info_t Init_info, char url[]);
int VITA_DM_Open(DM_Open_Info_t Open_info);
int VITA_DM_Scan(int Handle, DM_Scan_cmd_data scanData);
void VITA_DM_Abort(DM_Scan_cmd_data scanData);
void VITA_DM_SendResponse(char response[]);
char* VITA_DM_getTimeStamp(VITA_DM_DEVICETYPE_e scanType);

//!CURL Post data

struct curl_fetch_st {
    char *payload;
    size_t size;
};

size_t curl_callback (void *contents, size_t size, size_t nmemb, void *userp);
CURLcode curl_fetch_url(CURL *ch, const char *url, struct curl_fetch_st *fetch);
int VITA_DM_sendData(json_object *jsonStr,char *url);


//!IOTVITY

int VITA_DM_Init_ext(DM_Init_Info_t Init_info, char url[]);
int VITA_DM_Discover(void);
void VITA_DM_IoTvity_Get(const char *handle);
void VITA_DM_IoTvity_Put(const char *handle,json_object *obj);
void VITA_DM_IoTvity_Observe(const char *handle);



//!

#ifdef __cplusplus
}
#endif

#endif //VITA_DM_H
