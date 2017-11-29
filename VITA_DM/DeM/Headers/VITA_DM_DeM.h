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
#ifndef VT_DM_DEM_H
#define VT_DM_DEM_H

#include "VITA_DM_INTERNAL.h"

typedef char * DM_DeM_Init_Info_t;
typedef char * DM_DeM_Open_Info_t;
typedef char * DM_DeM_Scan_WiFi_Info_t;
typedef char * DM_DeM_Scan_BLE_Info_t;
typedef char * DM_DeM_Scan_BT_Info_t;


int VITA_DM_DeM_Init(DM_DeM_Init_Info_t Init_info);
int VITA_DM_DeM_Open(DM_DeM_Open_Info_t Open_info);
int VITA_DM_DeM_ScanWiFi(int Handle, DM_DeM_Scan_WiFi_Info_t Scan_info, DM_WIFI_ScanResult_Info_t *resultWIFIData);
int VITA_DM_DeM_ScanBT(int Handle, DM_DeM_Scan_BT_Info_t Scan_info, DM_BT_ScanResult_Info_t *resultBTData);
int VITA_DM_DeM_ScanBLE(int Handle, DM_DeM_Scan_BLE_Info_t Scan_info,DM_BLE_ScanResult_Info_t *resultBLEData);

extern int DM_BT_Scan(DM_BT_ScanResult_Info_t *resultData);
extern int DM_Wifi_Scan(DM_WIFI_ScanResult_Info_t *resultData);
extern int DM_BLE_Scan(DM_BLE_ScanResult_Info_t *resultData);


#endif //!VT_DM_DEM_H
