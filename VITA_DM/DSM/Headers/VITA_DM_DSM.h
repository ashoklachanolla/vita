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
#ifndef VITA_DM_DSM_H
#define VITA_DM_DSM_H

#include <stddef.h>
#include <json-c/json.h>
#include <curl/curl.h>

#include "../Headers/VITA_DM_INTERNAL.h"



/* TBD */
int VITA_DM_DaS_Format(void);
int VITA_DM_DaS_Convert(void);
//int VITA_DM_Das_Send(DM_BT_ScanResult_Info_t *BTSearchData, DM_WIFI_ScanResult_Info_t *WIFISearchData);



//!WIFI DSM
json_object *wifi_prepare_url(DM_WIFI_ScanResult_Info_t *WIFISearchData);
int VITA_DM_Das_WIFI_Send(DM_WIFI_ScanResult_Info_t *WIFISearchData);

//!BT DSM
json_object *bt_prepare_url(DM_BT_ScanResult_Info_t *BTSearchData);
int VITA_DM_Das_BT_Send(DM_BT_ScanResult_Info_t *BTSearchData);

//!BLE DSM
json_object *ble_prepare_url(DM_BLE_ScanResult_Info_t *BLESearchData);
int VITA_DM_Das_BLE_Send(DM_BLE_ScanResult_Info_t *BLESearchData);

//!IoTvity DSM
json_object *IoTvity_Disc_prepare_url(DM_IoTvity_discover_Info_t *IoTvityData , int Count);
int VITA_DM_DSM_IoTvity_Disc_Send(DM_IoTvity_discover_Info_t *IoTvityData , int Count);
int VITA_DM_DSM_IotVity_Get_Data (const char *handle, json_object *obj);
int VITA_DM_DSM_IotVity_Observe_Data (const char *handle, json_object *obj);


#endif //VITA_DM_DSM_H
