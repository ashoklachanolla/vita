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
/*************************************************************************************************************************
 **************************************************************************************************************************
 * NAME                  :   VITA_DM_DeM_ScanWiFi(int Handle, DM_DeM_Scan_WiFi_Info_t Scan_info, DM_WIFI_ScanResult_Info_t *resultWIFIData)
 *
 * DESCRIPTION           :   Wifi Scan API at DeM layer, triggers wifi scan api and gets the wifi result data
 *
 * INPUTS                :
 *                           INPUT PARAMETERS:
 *
 *                                           NAME :  Handle 	TYPE : int
 *                                           DESC :
 *                                          
 *                                           NAME :  Scan_info 	TYPE : DM_DeM_Scan_WiFi_Info_t
 *                                           DESC :  It contains wifi scan params to include
 *                                          
 *                           OUTPUT PARAMETERS:
 *
 *                                           NAME :  resultWIFIData 	TYPE : DM_WIFI_ScanResult_Info_t
 *                                           DESC :  It contains wifi scan results
 **************************************************************************************************************************
 **************************************************************************************************************************/

int VITA_DM_DeM_ScanWiFi(int Handle, DM_DeM_Scan_WiFi_Info_t Scan_info, DM_WIFI_ScanResult_Info_t *resultWIFIData)
{
    int result, timedout;
	result = DM_Wifi_Scan(resultWIFIData);
    if(result == -1) {
        return result;
    }

    timedout = TIMEOUT;

    while(result == 0) //! Timeout after MAXIMUM retries
    {  
        if(!timedout)
        {
            break;
        }
        //!To handle garbage memory if at all
        memset(resultWIFIData, 0, sizeof(resultWIFIData));

        result = DM_Wifi_Scan(resultWIFIData);
        timedout--;
        sleep(2);
    }

    if(timedout == 0)
    {        
        LOG_PRINT("Unable to find any WIFI devices/hotspots in vicinity\n");
        return 0;
    }

    return result;
}
