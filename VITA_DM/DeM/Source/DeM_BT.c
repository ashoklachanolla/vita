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
 * NAME                  :   VITA_DM_DeM_ScanBT(int Handle, DM_DeM_Scan_BT_Info_t Scan_info, DM_BT_ScanResult_Info_t *resultBTData)
 *
 * DESCRIPTION           :   Bluetooth Scan API at DeM layer, triggers bluetooth scan api and gets the wifi result data
 *
 * INPUTS                :
 *                           INPUT PARAMETERS:
 *
 *                                           NAME :  Handle 	TYPE : int
 *                                           DESC :
 *
 *                                           NAME :  Scan_info 	TYPE : DM_DeM_Scan_BT_Info_t
 *                                           DESC :  It contains bluetooth scan params to include
 *
 *                           OUTPUT PARAMETERS:
 *
 *                                           NAME :  resultBTData 	TYPE : DM_BT_ScanResult_Info_t
 *                                           DESC :  It contains wifi scan results
 **************************************************************************************************************************
 **************************************************************************************************************************/

int VITA_DM_DeM_ScanBT(int Handle, DM_DeM_Scan_BT_Info_t Scan_info, DM_BT_ScanResult_Info_t *resultBTData)
{
    int result, timedout;
    
	result = DM_BT_Scan(resultBTData);
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
        memset(resultBTData, 0, sizeof(resultBTData));

        result = DM_BT_Scan(resultBTData);
        timedout--;
        sleep(2);
    }

    if(timedout == 0)
    {
        LOG_PRINT("Unable to find any BT devices in vicinity");
        return 0;
    }

    return result;
}
