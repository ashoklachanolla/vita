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
#include <string.h>

#include "VITA_DM_DeM.h"
#include "helper.h"

int VITA_DM_DeM_ScanBLE(int Handle, DM_DeM_Scan_BLE_Info_t Scan_info, DM_BLE_ScanResult_Info_t *resultBLEData)
{
    int result, timedout;

    result = DM_BLE_Scan(resultBLEData);
    if(result == -1) {
        return result;
    }

    timedout = BLE_TIMEOUT;

    while(result == 0) //! Timeout after MAXIMUM retries
    {
        if(!timedout)
        {
            break;
        }
        //!To handle garbage memory if at all
        memset(resultBLEData, 0, sizeof(resultBLEData));

        result = DM_BLE_Scan(resultBLEData);
        if(result > 0)
        {
            break;
        }
        timedout--;
        sleep(2);
    }

    if(timedout == 0)
    {
       // LOG_PRINT("Unable to find any BLE devices in vicinity");
        printf("=================================================\n");
        LOG_PRINT("BLE SEARCH : NO DEVICES FOUND\n");
        printf("=================================================\n\n");
        return 0;
    }

    return result;
}
