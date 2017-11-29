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
#include <semaphore.h>
#include <pthread.h>

#include "VITA_DM_DSM.h"
#include "VITA_DM_INTERNAL.h"
#include "helper.h"

//!Bluetooth data structure
DM_BLE_ScanResult_Info_t BLEresult[MAX_SCAN_ELEMENTS];

sem_t ble_sem;
int ble_state;
bool bleAbortStatus = false;
VITA_SCAN_Type sBLEType = 0;
pthread_t ble_thread;



Scan_state_t get_ble_FSM_State()
{
    return ble_state;
}



void *BLE_Scan_Task(void *input)
{
    int result, count;
    bool contState;

    ble_state = STATE_INIT;
    contState = false;

    LOG_PRINT("Hello World! It's me, BLE thread #%d!", ble_state);

    while(1)
    {        
        if(contState)
        {            
            LOG_PRINT("BLE Task waiting for input.... ");
            sem_wait(&ble_sem);
        }

        switch (ble_state) {
        case STATE_INIT:
            LOG_PRINT("CASE STATE_INIT");
            ble_state = STATE_READY;
            contState = true;            
            break;

        case STATE_READY:
            LOG_PRINT("CASE STATE_READY");
            contState = false;
            //printf("<%s:%d>ScanType:%d\n", __func__, __LINE__, sBLEType);
            LOG_PRINT("ScanType:%d\n", sBLEType);
            if(sBLEType == AUTO)
            {
                ble_state = STATE_SCANNING_AUTO;
                LOG_PRINT("calling AUTO");
            }
            else if(sBLEType == MANUAL)
            {
                ble_state = STATE_SCANNING_MANUAL;
                LOG_PRINT("calling MANUAL");
            }
            else //!In any invalid input we go to READY State
            {
                ble_state = STATE_READY;
                contState = true;
            }
            LOG_PRINT("");
            break;

        case STATE_SCANNING_MANUAL:
            LOG_PRINT("CASE STATE_SCANNING_MANUAL");
            memset(BLEresult, 0, sizeof(BLEresult));
            count = VITA_DM_DeM_ScanBLE(1, 0, BLEresult);

            if(count == -1) {
                contState = true;
                ble_state = STATE_READY;
                VITA_DM_BLE_PostResponce(MANUAL, VITA_DM_BLE_SCAN_FAIL);
            }
            else if (count == 0) { //!Returning because already we had #MAXIMUM retries
                VITA_DM_BLE_PostResponce(MANUAL, VITA_DM_BLE_SCAN_NO_DEVICE_FOUND);
            }            
            else //! Scan Success and sending Data to SM
            {
                result = VITA_DM_Das_BLE_Send(BLEresult);

                if(result == 0 )
                    VITA_DM_BLE_PostResponce(MANUAL, VITA_DM_BLE_SCAN_SUCESS);
                else
                    VITA_DM_BLE_PostResponce(MANUAL, VITA_DM_BLE_SCAN_STORE_FAIL);
            }

            contState = true;
            ble_state = STATE_READY;
            break;

        case STATE_SCANNING_AUTO:
           // LOG_PRINT("CASE STATE_SCANNING_AUTO");
            memset(BLEresult, 0, sizeof(BLEresult));
            count = VITA_DM_DeM_ScanBLE(1, 0, BLEresult);

            if(bleAbortStatus == false) {
                if(count == -1) {
                    contState = true;
                    ble_state = STATE_READY;
                    VITA_DM_BLE_PostResponce(AUTO, VITA_DM_BLE_SCAN_FAIL);
                }
                else if (count == 0) { //!Returning because already we had #MAXIMUM retries
                    VITA_DM_BLE_PostResponce(AUTO, VITA_DM_BLE_SCAN_NO_DEVICE_FOUND);
                }
                else //! Scan Success and sending Data to SM
                {
                    result = VITA_DM_Das_BLE_Send(BLEresult);

                    if(result == 0 )
                        VITA_DM_BLE_PostResponce(AUTO, VITA_DM_BLE_SCAN_SUCESS);
                    else
                        VITA_DM_BLE_PostResponce(AUTO, VITA_DM_BLE_SCAN_STORE_FAIL);
                }
            }
            else
            {
                contState = true;
                ble_state = STATE_READY;
                VITA_DM_BLE_PostResponce(AUTO, VITA_DM_BLE_SCAN_ABORTED);
            }
            break;
        }        
    }
    pthread_exit(NULL);
}


int VITA_DM_BLE_Scan(VITA_SCAN_Type scanType)
{
    sBLEType = scanType;
    bleAbortStatus = false;

    if(ble_state == STATE_READY)
    {
        sem_post(&ble_sem);
    }
    else if(ble_state == STATE_SCANNING_AUTO || ble_state == STATE_SCANNING_MANUAL)
    {
        VITA_DM_BLE_PostResponce(scanType, VITA_DM_BLE_SCAN_PROGRESS);
        return 1;
    }

    return 0;
}

void VITA_DM_BLE_Abort(void)
{
    bleAbortStatus = true;
    sBLEType = 0;
}



int VITA_DM_BLE_Init()
{
    int res;

    //!BLE Semaphore
    res = sem_init(&ble_sem, 0, 0);
    if (res != 0) {
        perror("Semaphore initialization failed");        
        return -1;
    }

    //!BLE Task creation
    res = pthread_create(&ble_thread, NULL, BLE_Scan_Task, NULL);
    if (res != 0) {
        perror("BLE Task creation failed");
        return -1;
    }

    return 0;
}
