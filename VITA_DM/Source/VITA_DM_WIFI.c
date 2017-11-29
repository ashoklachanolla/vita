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
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#include "VITA_DM_INTERNAL.h"
#include "helper.h"

//!WIFI data structure
DM_WIFI_ScanResult_Info_t WIFIresult[MAX_SCAN_ELEMENTS];
sem_t wifi_sem;
int wifi_state;
bool wifiAbortStatus = false;
VITA_SCAN_Type sWIFIType = 0;
pthread_t wifi_thread;

/*************************************************************************************************************************
 **************************************************************************************************************************
 * NAME                  :   get_wifi_FSM_State()
 *
 * DESCRIPTION           :   To Get Wifi Finate State Machine State
 *
 * INPUTS                :
 *                           INPUT PARAMETERS:
 *
 *                                           NAME :   	TYPE :
 *                                           DESC :
 *
 *                                           NAME : bt_response  	TYPE : DM_WIFI_SCAN_RESPONCE_e
 *                                           DESC : enum for init status response i.e success/failure
 *
 *                           OUTPUT PARAMETERS:
 *
 *                                           NAME : Scan_state_t  	TYPE :
 *                                           DESC :
 **************************************************************************************************************************
 **************************************************************************************************************************/

Scan_state_t get_wifi_FSM_State()
{
    return wifi_state;
}

/*************************************************************************************************************************
 **************************************************************************************************************************
 * NAME                  :   wifi_task(void)
 *
 * DESCRIPTION           :   To Iniate Wifi task
 *
 * INPUTS                :
 *                           INPUT PARAMETERS:
 *
 *                                           NAME :   	TYPE :
 *                                           DESC :
 *
 *                           OUTPUT PARAMETERS:
 *
 *                                           NAME : Scan_state_t  	TYPE :
 *                                           DESC :
 **************************************************************************************************************************
 **************************************************************************************************************************/
void *WIFI_Scan_Task(void *input)
{    
    int result, count;
    bool contState;

    wifi_state = STATE_INIT;
    contState = false;

    LOG_PRINT("Hello World! It's me, WIFI thread #%d!", wifi_state);

    while(1)
    {
        if(contState)
        {            
            LOG_PRINT("WIFI Task waiting for input.... ");
            sem_wait(&wifi_sem);            

        }

        switch (wifi_state) {
        case STATE_INIT:
            LOG_PRINT("CASE STATE_INIT");
            wifi_state = STATE_READY;
            contState = true;
            break;

        case STATE_READY:
            LOG_PRINT("CASE STATE_READY");
            contState = false;

            if(sWIFIType == AUTO)
            {
                wifi_state = STATE_SCANNING_AUTO;
                LOG_PRINT("calling AUTO");
            }
            else if(sWIFIType == MANUAL)
            {
                wifi_state = STATE_SCANNING_MANUAL;
                LOG_PRINT("calling MANUAL");
            }
            else //!In any invalid input we go to READY State
            {
                wifi_state = STATE_READY;
                contState = true;
            }

            break;

        case STATE_SCANNING_MANUAL:
            LOG_PRINT("CASE STATE_SCANNING_MANUAL");
            memset(WIFIresult, 0, sizeof(WIFIresult));
            count = VITA_DM_DeM_ScanWiFi(1, 0, WIFIresult);

            if(count == -1) {
                contState = true;
                wifi_state = STATE_READY;
                VITA_DM_WiFi_PostResponce(MANUAL, VITA_DM_WIFI_SCAN_FAIL);
            }
            else if (count == 0) { //!Returning because already we had #MAXIMUM retries
                VITA_DM_WiFi_PostResponce(MANUAL, VITA_DM_WIFI_SCAN_NO_DEVICE_FOUND);
            }
            else //! Scan Success and sending Data to SM
            {
                result = VITA_DM_Das_WIFI_Send(WIFIresult);

                if(result == 0 )
                    VITA_DM_WiFi_PostResponce(MANUAL, VITA_DM_WIFI_SCAN_SUCESS);
                else
                    VITA_DM_WiFi_PostResponce(MANUAL, VITA_DM_WIFI_SCAN_STORE_FAIL);
            }

            contState = true;
            wifi_state = STATE_READY;
            break;

        case STATE_SCANNING_AUTO:
            LOG_PRINT("CASE STATE_SCANNING_AUTO");
            memset(WIFIresult, 0, sizeof(WIFIresult));
            count = VITA_DM_DeM_ScanWiFi(1, 0, WIFIresult);

            if(wifiAbortStatus == false)
            {
                if(count == -1) {
                    contState = true;
                    wifi_state = STATE_READY;
                    VITA_DM_WiFi_PostResponce(AUTO, VITA_DM_WIFI_SCAN_FAIL);
                }
                else if (count == 0) { //!Returning because already we had #MAXIMUM retries
                    VITA_DM_WiFi_PostResponce(AUTO, VITA_DM_WIFI_SCAN_NO_DEVICE_FOUND);
                }
                else //! Scan Success and sending Data to SM
                {
                    result = VITA_DM_Das_WIFI_Send(WIFIresult);

                    if(result == 0 )
                        VITA_DM_WiFi_PostResponce(AUTO, VITA_DM_WIFI_SCAN_SUCESS);
                    else
                        VITA_DM_WiFi_PostResponce(AUTO, VITA_DM_WIFI_SCAN_STORE_FAIL);
                }
            }
            else
            {
                contState = true;
                wifi_state = STATE_READY;
                VITA_DM_WiFi_PostResponce(AUTO, VITA_DM_WIFI_SCAN_ABORTED);
            }

            sleep(7);
            break;
        }
        LOG_PRINT("<%s:%d>", __func__, __LINE__);
    }
    pthread_exit(NULL);
}

/*************************************************************************************************************************
 **************************************************************************************************************************
 * NAME                  :   VITA_DM_WIFI_Scan(void)
 *
 * DESCRIPTION           :   To Iniate Wifi task
 *
 * INPUTS                :
 *                           INPUT PARAMETERS:
 *
 *                                           NAME :   	TYPE :
 *                                           DESC :
 *
 *                           OUTPUT PARAMETERS:
 *
 *                                           NAME : Scan_state_t  	TYPE :
 *                                           DESC :
 **************************************************************************************************************************
 **************************************************************************************************************************/

int VITA_DM_WIFI_Scan(VITA_SCAN_Type scanType)
{
    sWIFIType = scanType;
    wifiAbortStatus = false;

    if(wifi_state == STATE_READY)
    {
        sem_post(&wifi_sem);
    }
    else if(wifi_state == STATE_SCANNING_AUTO || wifi_state == STATE_SCANNING_MANUAL)
    {
        VITA_DM_WiFi_PostResponce(scanType, VITA_DM_WIFI_SCAN_PROGRESS);
        return 1;
    }

    return 0;
}

void VITA_DM_WIFI_Abort(void)
{
    wifiAbortStatus = true;
    sWIFIType = 0;
}

/*************************************************************************************************************************
 **************************************************************************************************************************
 * NAME                  :   VITA_DM_WIFI_Init()
 *
 * DESCRIPTION           :   To Iniate Wifi task
 *
 * INPUTS                :
 *                           INPUT PARAMETERS:
 *
 *                                           NAME :   	TYPE :
 *                                           DESC :
 *
 *                           OUTPUT PARAMETERS:
 *
 *                                           NAME : Scan_state_t  	TYPE :
 *                                           DESC :
 **************************************************************************************************************************
 **************************************************************************************************************************/

int VITA_DM_WIFI_Init()
{
    int res;

    //!WIFI Semaphore
    res = sem_init(&wifi_sem, 0, 0);
    if (res != 0) {
        perror("Semaphore initialization failed");
        exit(EXIT_FAILURE);
    }

    //!WIFI Task
    res = pthread_create(&wifi_thread, NULL, WIFI_Scan_Task, NULL);
    if (res != 0) {
        perror("WIFI Task creation failed");
        exit(EXIT_FAILURE);
    }
    return 0;
}
