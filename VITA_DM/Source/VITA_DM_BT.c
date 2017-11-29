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
DM_BT_ScanResult_Info_t BTresult[MAX_SCAN_ELEMENTS];

sem_t bt_sem;
int bt_state;
bool btAbortStatus = false;
VITA_SCAN_Type sBTType = 0;
pthread_t bt_thread;

/*************************************************************************************************************************
 **************************************************************************************************************************
 * NAME                  :   get_bt_FSM_State()
 *
 * DESCRIPTION           :   To Get Bluetooth Finate State Machine State
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

Scan_state_t get_bt_FSM_State()
{
    return bt_state;
}

/*************************************************************************************************************************
 **************************************************************************************************************************
 * NAME                  :   bt_task(void)
 *
 * DESCRIPTION           :
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

void *BT_Scan_Task(void *input)
{
    int result, count;
    bool contState;

    bt_state = STATE_INIT;
    contState = false;

    //printf("Hello World! It's me, BT thread #%d!\n", bt_state);
    LOG_PRINT("Hello World! It's me, BT thread #%d!", bt_state);

    while(1)
    {        
        if(contState)
        {            
            //printf("BT Task waiting for input.... \n");
            LOG_PRINT("BT Task waiting for input.... ");
            sem_wait(&bt_sem);
        }

        switch (bt_state) {
        case STATE_INIT:
            LOG_PRINT("CASE STATE_INIT");
            bt_state = STATE_READY;
            contState = true;            
            break;

        case STATE_READY:
            LOG_PRINT("CASE STATE_READY");
            contState = false;
            //printf("<%s:%d>ScanType:%d\n", __func__, __LINE__, sBTType);
            LOG_PRINT("ScanType:%d\n", sBTType);
            if(sBTType == AUTO)
            {
                bt_state = STATE_SCANNING_AUTO;
                LOG_PRINT("calling AUTO");
            }
            else if(sBTType == MANUAL)
            {
                bt_state = STATE_SCANNING_MANUAL;
                LOG_PRINT("calling MANUAL");
            }
            else //!In any invalid input we go to READY State
            {
                bt_state = STATE_READY;
                contState = true;
            }
            LOG_PRINT("");
            break;

        case STATE_SCANNING_MANUAL:
            LOG_PRINT("CASE STATE_SCANNING_MANUAL");
            memset(BTresult, 0, sizeof(BTresult));
            count = VITA_DM_DeM_ScanBT(1, 0, BTresult);

            if(count == -1) {
                contState = true;
                bt_state = STATE_READY;
                VITA_DM_BT_PostResponce(MANUAL, VITA_DM_BT_SCAN_FAIL);
            }
            else if (count == 0) { //!Returning because already we had #MAXIMUM retries
                VITA_DM_BT_PostResponce(MANUAL, VITA_DM_BT_SCAN_NO_DEVICE_FOUND);                
            }            
            else //! Scan Success and sending Data to SM
            {
                result = VITA_DM_Das_BT_Send(BTresult);

                if(result == 0 )
                    VITA_DM_BT_PostResponce(MANUAL, VITA_DM_BT_SCAN_SUCESS);
                else
                    VITA_DM_BT_PostResponce(MANUAL, VITA_DM_BT_SCAN_STORE_FAIL);
            }

            contState = true;
            bt_state = STATE_READY;
            break;

        case STATE_SCANNING_AUTO:
            LOG_PRINT("CASE STATE_SCANNING_AUTO");
            memset(BTresult, 0, sizeof(BTresult));
            count = VITA_DM_DeM_ScanBT(1, 0, BTresult);

            if(btAbortStatus == false) {
                if(count == -1) {
                    contState = true;
                    bt_state = STATE_READY;
                    VITA_DM_BT_PostResponce(AUTO, VITA_DM_BT_SCAN_FAIL);
                }
                else if (count == 0) { //!Returning because already we had #MAXIMUM retries
                    VITA_DM_BT_PostResponce(AUTO, VITA_DM_BT_SCAN_NO_DEVICE_FOUND);
                }
                else //! Scan Success and sending Data to SM
                {
                    result = VITA_DM_Das_BT_Send(BTresult);

                    if(result == 0 )
                        VITA_DM_BT_PostResponce(AUTO, VITA_DM_BT_SCAN_SUCESS);
                    else
                        VITA_DM_BT_PostResponce(AUTO, VITA_DM_BT_SCAN_STORE_FAIL);
                }
            }
            else
            {
                contState = true;
                bt_state = STATE_READY;
                VITA_DM_BT_PostResponce(AUTO, VITA_DM_BT_SCAN_ABORTED);
            }
            break;
        }        
    }
    pthread_exit(NULL);
}

/*************************************************************************************************************************
 **************************************************************************************************************************
 * NAME                  :   VITA_DM_BT_Scan(void)
 *
 * DESCRIPTION           :
 *
 * INPUTS                :
 *                           INPUT PARAMETERS:
 *
 *                                           NAME :   	TYPE :
 *                                           DESC :
 *
 *                           OUTPUT PARAMETERS:
 *
 *                                           NAME :   	TYPE :
 *                                           DESC :
 **************************************************************************************************************************
 **************************************************************************************************************************/

int VITA_DM_BT_Scan(VITA_SCAN_Type scanType)
{
    sBTType = scanType;
    btAbortStatus = false;

    if(bt_state == STATE_READY)
    {
        sem_post(&bt_sem);
    }
    else if(bt_state == STATE_SCANNING_AUTO || bt_state == STATE_SCANNING_MANUAL)
    {
        VITA_DM_BT_PostResponce(scanType, VITA_DM_BT_SCAN_PROGRESS);
        return 1;
    }

    return 0;
}

void VITA_DM_BT_Abort(void)
{
    btAbortStatus = true;
    sBTType = 0;
}

/*************************************************************************************************************************
 **************************************************************************************************************************
 * NAME                  :   VITA_DM_BT_Init()
 *
 * DESCRIPTION           :
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

int VITA_DM_BT_Init()
{
    int res;

    //!BT Semaphore
    res = sem_init(&bt_sem, 0, 0);
    if (res != 0) {
        perror("Semaphore initialization failed");        
        return -1;
    }

    //!BT Task creation
    res = pthread_create(&bt_thread, NULL, BT_Scan_Task, NULL);
    if (res != 0) {
        perror("BT Task creation failed");        
        return -1;
    }

    return 0;
}
