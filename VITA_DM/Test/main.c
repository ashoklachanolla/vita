#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include "helper.h"

#include "TEST_HEADER.h"

#define SLEEP_TIME_TO_TEST  5

int testcase = 0;
int autoScanCount = 0;

static int hDMHandle;
int input();
void resultHandler(int count);

int input()
{
    DM_Scan_cmd_data scanData;
    int option = 0, result = -1;
    LOG_PRINT("User selected: %d\n", option);
    LOG_PRINT("Please choose option to search\n");
    LOG_PRINT("1 Bluetooth Scan:\n\
              \r2 WIFI Scan:\n\
              \r3 Continuous scan:\n");
    scanf("%d", &option);
    switch (option) {
        case 1:
            LOG_PRINT("Scanning Bluetooth Devices....\n");
            scanData.deviceType = DEVICE_BLUETOOTH;
            scanData.scanType = AUTO;
            strcpy(scanData.cmd,"VITA_DM_Scan");
            
            result = VITA_DM_Scan(1,scanData);
            break;
        case 2:
            LOG_PRINT("Scanning Wifi Devices....\n");
            scanData.deviceType = DEVICE_WIFI;
            scanData.scanType = AUTO;
            strcpy(scanData.cmd,"VITA_DM_Scan");
            
            result = VITA_DM_Scan(1,scanData);
            break;
        case 3:
            /*do {
             LOG_PRINT("Scanning Bluetooth Devices....\n");
             result = VITA_DM_Scan(hDMHandle, DEVICE_BLUETOOTH);
             LOG_PRINT("Scanning Wifi Devices....\n");
             wait(500);
             result = VITA_DM_Scan(hDMHandle, DEVICE_WIFI);
             } while(1);*/
            break;
        default:
            LOG_PRINT("Invalid option. Please try again.\n");
            input();
    }
    return result;
}

void resultHandler(int count)
{
    switch (count)
    {
        case -1:
            LOG_PRINT("Unable to scan, check whether host has BT/WIFI Adapter\n");
            break;
        case 0:
            LOG_PRINT("SUCCESS SCAN\n");
            break;
        case 1:
            LOG_PRINT("No Devices found.\n");
            break;
        case 2:
            LOG_PRINT("CUSTOM result:%d \n", count);
            break;
        case 3:
            LOG_PRINT("CUSTOM result:%d \n", count);
        default:
            break;
    }
}

void VITA_DM_TEST_CASE_2()
{
    LOG_PRINT("********************** VITA_DM_TEST_CASE_2 **********************\n");
    DM_Init_Info_t dminfo;
    
    testcase = 2;
    
    dminfo.BT_VITA_Filter.DM_BT_DSM_Filter.bIncludeBtName = 1;
    dminfo.BT_VITA_Filter.DM_BT_DSM_Filter.bIncludeBtDeviceID = 1;
    dminfo.BT_VITA_Filter.DM_BT_DSM_Filter.bIncludeBtClass = 1;
    
    dminfo.WIFI_VITA_Filter.DM_WIFI_DSM_Filter.bIncludeWifiName = 1;
    dminfo.WIFI_VITA_Filter.DM_WIFI_DSM_Filter.bIncludeWifiSignal = 1;
    dminfo.WIFI_VITA_Filter.DM_WIFI_DSM_Filter.bIncludeWifiDeviceID = 1;
    dminfo.WIFI_VITA_Filter.DM_WIFI_DSM_Filter.bIncludeWifiFrequency = 1;
    
    strcpy(dminfo.DeviceId,"B8:27:EB:39:1A:2F");
    strcpy(dminfo.SM_URL,"http://192.168.4.19:8856/iotpoc/user/addUserDeviceDetails");
    
    VITA_DM_Init(dminfo,dminfo.SM_URL);
}


void VITA_DM_TEST_CASE_6()
{
    LOG_PRINT("********************** VITA_DM_TEST_CASE_6 **********************\n");
    testcase = 6;
    
    DM_Scan_cmd_data scanData;
    scanData.deviceType = DEVICE_WIFI;
    scanData.scanType = MANUAL;
    strcpy(scanData.cmd,"VITA_DM_Scan");
    //    VITA_DM_SetScanParameters(scanData);
    
    VITA_DM_Scan(1,scanData);
}

void VITA_DM_TEST_CASE_7()
{
    LOG_PRINT("********************** VITA_DM_TEST_CASE_7 **********************\n");
    testcase = 7;
    
    DM_Scan_cmd_data scanData;
    scanData.deviceType = DEVICE_WIFI;
    scanData.scanType = MANUAL;
    strcpy(scanData.cmd,"VITA_DM_Scan");
    //    VITA_DM_SetScanParameters(scanData);
    
    VITA_DM_Scan(1,scanData);
}

void VITA_DM_TEST_CASE_8()
{
    LOG_PRINT("********************** VITA_DM_TEST_CASE_8 **********************\n");
    testcase = 8;
    autoScanCount = 0;
    
    DM_Scan_cmd_data scanData;
    scanData.deviceType = DEVICE_WIFI;
    scanData.scanType = AUTO;
    strcpy(scanData.cmd,"VITA_DM_Scan");
    //    VITA_DM_SetScanParameters(scanData);
    
    VITA_DM_Scan(1,scanData);
}


void VITA_DM_TEST_CASE_10()
{
    LOG_PRINT("********************** VITA_DM_TEST_CASE_10 **********************\n");
    testcase = 10;
    
    DM_Scan_cmd_data scanData;
    scanData.deviceType = DEVICE_BLUETOOTH;
    scanData.scanType = MANUAL;
    strcpy(scanData.cmd,"VITA_DM_Scan");
    
    //    VITA_DM_SetScanParameters(scanData);
    
    VITA_DM_Scan(1,scanData);
}


void VITA_DM_TEST_CASE_11()
{
    LOG_PRINT("********************** VITA_DM_TEST_CASE_11 **********************\n");
    testcase = 11;
    
    DM_Scan_cmd_data scanData;
    scanData.deviceType = DEVICE_BLUETOOTH;
    scanData.scanType = MANUAL;
    strcpy(scanData.cmd ,"VITA_DM_Scan");
    
    //    VITA_DM_SetScanParameters(scanData);
    
    VITA_DM_Scan(1,scanData);
}

void VITA_DM_TEST_CASE_12()
{
    LOG_PRINT("********************** VITA_DM_TEST_CASE_12 **********************\n");
    LOG_PRINT("%s:%d\n", __func__, __LINE__);
    testcase = 12;
    autoScanCount = 0;
    
    DM_Scan_cmd_data scanData;
    scanData.deviceType = DEVICE_BLUETOOTH;
    scanData.scanType = AUTO;
    strcpy(scanData.cmd,"VITA_DM_Scan");
    //VITA_DM_SetScanParameters(scanData);
    
    VITA_DM_Scan(1,scanData);
}

void VITA_DM_TEST_CASE_14()
{
    LOG_PRINT("********************** VITA_DM_TEST_CASE_14 **********************\n");
    testcase = 14;
    
    DM_Init_Info_t dminfo;
    
    dminfo.BT_VITA_Filter.DM_BT_DSM_Filter.bIncludeBtName = 1;
    dminfo.BT_VITA_Filter.DM_BT_DSM_Filter.bIncludeBtDeviceID = 1;
    dminfo.BT_VITA_Filter.DM_BT_DSM_Filter.bIncludeBtClass = 1;
    
    dminfo.WIFI_VITA_Filter.DM_WIFI_DSM_Filter.bIncludeWifiName = 1;
    dminfo.WIFI_VITA_Filter.DM_WIFI_DSM_Filter.bIncludeWifiSignal = 1;
    dminfo.WIFI_VITA_Filter.DM_WIFI_DSM_Filter.bIncludeWifiDeviceID = 1;
    dminfo.WIFI_VITA_Filter.DM_WIFI_DSM_Filter.bIncludeWifiFrequency = 1;
    
    strcpy(dminfo.DeviceId,"B8:27:EB:39:1A:2F");
    strcpy(dminfo.SM_URL,"htt://192.16.5.19:8856/iotpoc/user/addUserDeviceDetails");
    
    VITA_DM_Init(dminfo,dminfo.SM_URL);
    
    DM_Scan_cmd_data scanData;
    scanData.deviceType = DEVICE_WIFI;
    scanData.scanType = MANUAL;
    strcpy(scanData.cmd,"VITA_DM_Scan");
    
    //    VITA_DM_SetScanParameters(scanData);
    
    VITA_DM_Scan(1,scanData);
    
}

void VITA_DM_TEST_CASE_15()
{
    LOG_PRINT("********************** VITA_DM_TEST_CASE_15 **********************\n");
    testcase = 15;
    
    DM_Init_Info_t dminfo;
    
    dminfo.BT_VITA_Filter.DM_BT_DSM_Filter.bIncludeBtName = 1;
    dminfo.BT_VITA_Filter.DM_BT_DSM_Filter.bIncludeBtDeviceID = 1;
    dminfo.BT_VITA_Filter.DM_BT_DSM_Filter.bIncludeBtClass = 1;
    
    dminfo.WIFI_VITA_Filter.DM_WIFI_DSM_Filter.bIncludeWifiName = 1;
    dminfo.WIFI_VITA_Filter.DM_WIFI_DSM_Filter.bIncludeWifiSignal = 1;
    dminfo.WIFI_VITA_Filter.DM_WIFI_DSM_Filter.bIncludeWifiDeviceID = 1;
    dminfo.WIFI_VITA_Filter.DM_WIFI_DSM_Filter.bIncludeWifiFrequency = 1;
    
    strcpy(dminfo.DeviceId,"B8:27:EB:39:1A:2F");
    strcpy(dminfo.SM_URL,"htt://192.16.5.19:8856/iotpoc/user/addUserDeviceDetails");
    
    VITA_DM_Init(dminfo,dminfo.SM_URL);
    
    DM_Scan_cmd_data scanData;
    scanData.deviceType = DEVICE_BLUETOOTH;
    scanData.scanType = MANUAL;
    strcpy(scanData.cmd,"VITA_DM_Scan");
    
    //    VITA_DM_SetScanParameters(scanData);
    
    VITA_DM_Scan(1,scanData);
    
}

void VITA_DM_TEST_CASE_19()
{
    LOG_PRINT("********************** VITA_DM_TEST_CASE_19 **********************\n");
    testcase = 19;
    
    DM_Init_Info_t dminfo;
    
    dminfo.BT_VITA_Filter.DM_BT_DSM_Filter.bIncludeBtName = 1;
    dminfo.BT_VITA_Filter.DM_BT_DSM_Filter.bIncludeBtDeviceID = 1;
    dminfo.BT_VITA_Filter.DM_BT_DSM_Filter.bIncludeBtClass = 1;
    
    dminfo.WIFI_VITA_Filter.DM_WIFI_DSM_Filter.bIncludeWifiName = 1;
    dminfo.WIFI_VITA_Filter.DM_WIFI_DSM_Filter.bIncludeWifiSignal = 1;
    dminfo.WIFI_VITA_Filter.DM_WIFI_DSM_Filter.bIncludeWifiDeviceID = 1;
    dminfo.WIFI_VITA_Filter.DM_WIFI_DSM_Filter.bIncludeWifiFrequency = 1;
    
    strcpy(dminfo.DeviceId,"B8:27:EB:39:1A:2F");
    strcpy(dminfo.SM_URL,"http://192.168.4.19:8856/iotpoc/user/addUserDeviceDetails");
    
    VITA_DM_Init(dminfo,dminfo.SM_URL);
    
    DM_Scan_cmd_data scanData;
    scanData.deviceType = DEVICE_WIFI;
    scanData.scanType = AUTO;
    strcpy(scanData.cmd,"VITA_DM_Scan");
    //    VITA_DM_SetScanParameters(scanData);
    
    VITA_DM_Scan(1,scanData);
    
    DM_Scan_cmd_data abortdata;
    abortdata.deviceType = DEVICE_WIFI;
    abortdata.scanType = AUTO;
    strcpy(abortdata.cmd,"VITA_DM_Scan_Abort");
    
    //bAbortStatus = true;
    sleep(5);
    VITA_DM_Abort(abortdata);
    //    DM_SetAbortStatus(true);
    
}

void VITA_DM_TEST_CASE_20()
{
    LOG_PRINT("********************** VITA_DM_TEST_CASE_20 **********************\n");
    testcase = 20;
    
    DM_Scan_cmd_data scanData;
    scanData.deviceType = DEVICE_BLUETOOTH;
    scanData.scanType = AUTO;
    strcpy(scanData.cmd,"VITA_DM_Scan");
    
    VITA_DM_Scan(1,scanData);
    
    //bAbortStatus = true;
    DM_Scan_cmd_data abortdata;
    abortdata.deviceType = DEVICE_BLUETOOTH;
    abortdata.scanType = AUTO;
    strcpy(abortdata.cmd,"VITA_DM_Scan_Abort");
    sleep(5);
    //bAbortStatus = true;
    VITA_DM_Abort(abortdata);
}
void VITA_DM_TEST_CASE_22()
{
    LOG_PRINT("********************** VITA_DM_TEST_CASE_22 **********************\n");
    testcase = 22;
    
    DM_Scan_cmd_data scanData;
    scanData.deviceType = DEVICE_WIFI;
    scanData.scanType = AUTO;
    strcpy(scanData.cmd,"VITA_DM_Scan");
    
    VITA_DM_Scan(1,scanData);
    
    sleep(5);
    
    DM_Scan_cmd_data ManualScandata;
    ManualScandata.deviceType = DEVICE_WIFI;
    ManualScandata.scanType = MANUAL;
    strcpy(ManualScandata.cmd,"VITA_DM_Scan");
    //bAbortStatus = true;
    VITA_DM_Scan(1,ManualScandata);
    
    
    
}
void VITA_DM_TEST_CASE_23()
{
    LOG_PRINT("********************** VITA_DM_TEST_CASE_23 **********************\n");
    testcase = 23;
    
    DM_Scan_cmd_data scanData;
    scanData.deviceType = DEVICE_BLUETOOTH;
    scanData.scanType = AUTO;
    strcpy(scanData.cmd,"VITA_DM_Scan");
    
    VITA_DM_Scan(1,scanData);
    
    sleep(5);
    
    DM_Scan_cmd_data ManualScandata;
    ManualScandata.deviceType = DEVICE_BLUETOOTH;
    ManualScandata.scanType = MANUAL;
    strcpy(ManualScandata.cmd,"VITA_DM_Scan");
    //bAbortStatus = true;
    VITA_DM_Scan(1,ManualScandata);
}


void Handle_init_test_response(DM_INIT_RESPONCE_e init_response)
{
    if(testcase == 2)
    {
        if(init_response == VITA_DM_INIT_SUCESS)
        {
            LOG_PRINT("########## ************ VITA_DM_TESTCASE_2 : PASSED ************ ##########\n");
            testcase = 0;
        }
        else
        {
            LOG_PRINT("########## ************ VITA_DM_TESTCASE_2 : FAILED ************ ##########\n");
            testcase = 0;
        }
    }
}
//void Handle_BTScan_test_response(VITA_SCAN_Type scanType ,DM_BT_SCAN_RESPONCE_e bt_response)
void Handle_BTScan_test_response(VITA_SCAN_Type scanType ,DM_BT_SCAN_RESPONCE_e bt_response,char *scan_cmd)
{
    LOG_PRINT("BT <%s:%d>---- testcase = %d, response = %d cmd = %s \n", __func__, __LINE__,testcase,bt_response,scan_cmd);
    
    if(testcase == 10)
    {
        if(scanType == MANUAL)
        {
            if(bt_response == VITA_DM_BT_SCAN_SUCESS)
            {
                LOG_PRINT("########## ************ VITA_DM_TESTCASE_10 : PASSED ************ ##########\n");
                testcase = 0;
                
            }
            else
            {
                LOG_PRINT("########## ************ VITA_DM_TESTCASE_10 : FAILED ************ ##########\n");
                testcase = 0;
                
                
            }
        }
    }
    else  if(testcase == 11)
    {
        if(scanType == MANUAL)
        {
            if(bt_response == VITA_DM_BT_SCAN_NO_DEVICE_FOUND)
            {
                LOG_PRINT("########## ************ VITA_DM_TESTCASE_11 : PASSED ************ ##########\n");
                testcase = 0;
                
            }
            else
            {
                LOG_PRINT("########## ************ VITA_DM_TESTCASE_11 : FAILED ************ ##########\n");
                testcase = 0;
                
                
            }
        }
    }
    else if(testcase == 12)
    {
        if(scanType == AUTO)
        {
            if(bt_response == VITA_DM_BT_SCAN_SUCESS || bt_response == VITA_DM_BT_SCAN_NO_DEVICE_FOUND )
            {
                if(autoScanCount == 3)
                {
                    LOG_PRINT("########## ************ VITA_DM_TESTCASE_12 : PASSED ************ ##########\n");
                    testcase = 0;
                    
                    // bAbortStatus = true;
                    
                    DM_Scan_cmd_data abortdata;
                    abortdata.deviceType = DEVICE_BLUETOOTH;
                    abortdata.scanType = AUTO;
                    strcpy(abortdata.cmd,"VITA_DM_Scan_Abort");
                    
                    //bAbortStatus = true;
                    VITA_DM_Abort(abortdata);
                    // DM_SetAbortStatus(true);
                    
                    
                }
                autoScanCount ++;
            }
            else
            {
                LOG_PRINT("########## ************ VITA_DM_TESTCASE_12 : FAILED ************ ##########\n");
                testcase = 0;
                
                // bAbortStatus = true;
                
                DM_Scan_cmd_data abortdata;
                abortdata.deviceType = DEVICE_BLUETOOTH;
                abortdata.scanType = AUTO;
                strcpy(abortdata.cmd,"VITA_DM_Scan_Abort");
                
                //bAbortStatus = true;
                VITA_DM_Abort(abortdata);
                //  DM_SetAbortStatus(true);
                
            }
        }
    }
    else if(testcase == 15)
    {
        if(bt_response == VITA_DM_BT_SCAN_STORE_FAIL)
        {
            LOG_PRINT("########## ************ VITA_DM_TESTCASE_15 : PASSED ************ ##########\n");
            testcase = 0;
            
            
        }
        else
        {
            LOG_PRINT("########## ************ VITA_DM_TESTCASE_15 : FAILED ************ ##########\n");
            testcase = 0;
            
        }
    }
    else if(testcase == 20 && strcmp(scan_cmd, "VITA_DM_ScanAbort") == 0)
    {
        if(bt_response == VITA_DM_BT_SCAN_ABORTED)
        {
            LOG_PRINT("########## ************ VITA_DM_TESTCASE_20 : PASSED ************ ##########\n");
            testcase = 0;
            
            
        }
        else
        {
            LOG_PRINT("########## ************ VITA_DM_TESTCASE_20 : FAILED ************ ##########\n");
            testcase = 0;
            
        }
    }
    else if(testcase == 23 && scanType == MANUAL)
    {
        if(bt_response == VITA_DM_BT_SCAN_PROGRESS)
        {
            LOG_PRINT("########## ************ VITA_DM_TESTCASE_23 : PASSED ************ ##########\n");
            testcase = 0;
            
            
        }
        else
        {
            LOG_PRINT("########## ************ VITA_DM_TESTCASE_23 : FAILED ************ ##########\n");
            testcase = 0;
            
        }
        DM_Scan_cmd_data abortdata;
        abortdata.deviceType = DEVICE_BLUETOOTH;
        abortdata.scanType = AUTO;
        strcpy(abortdata.cmd,"VITA_DM_Scan_Abort");
        
        //bAbortStatus = true;
        VITA_DM_Abort(abortdata);
    }
    
}

void Handle_WIFIScan_test_response(VITA_SCAN_Type scanType ,DM_WIFI_SCAN_RESPONCE_e bt_response,char *scan_cmd)
{
    LOG_PRINT("Wifi <%s:%d>---- testcase = %d, response = %d cmd = %s \n", __func__, __LINE__,testcase,bt_response,scan_cmd);
    if(testcase == 6)
    {
        if(scanType == MANUAL)
        {
            if(bt_response == VITA_DM_WIFI_SCAN_SUCESS)
            {
                LOG_PRINT("########## ************ VITA_DM_TESTCASE_6 : PASSED ************ ##########\n");
                testcase = 0;
                
            }
            else
            {
                LOG_PRINT("########## ************ VITA_DM_TESTCASE_6 : FAILED ************ ##########\n");
                testcase = 0;
                
                
            }
        }
    }
    else  if(testcase == 7)
    {
        if(scanType == MANUAL)
        {
            if(bt_response == VITA_DM_WIFI_SCAN_NO_DEVICE_FOUND)
            {
                LOG_PRINT("########## ************ VITA_DM_TESTCASE_7 : PASSED ************ ##########\n");
                testcase = 0;
                
            }
            else
            {
                LOG_PRINT("########## ************ VITA_DM_TESTCASE_7 : FAILED ************ ##########\n");
                
                testcase = 0;
                
            }
        }
    }
    else if(testcase == 8)
    {
        if(scanType == AUTO)
        {
            if(bt_response == VITA_DM_WIFI_SCAN_SUCESS || bt_response == VITA_DM_WIFI_SCAN_NO_DEVICE_FOUND )
            {
                if(autoScanCount == 3)
                {
                    LOG_PRINT("########## ************ VITA_DM_TESTCASE_8 : PASSED ************ ##########\n");
                    testcase = 0;
                    
                    // bAbortStatus = true;
                    //                    DM_SetAbortStatus(true);
                    DM_Scan_cmd_data abortdata;
                    abortdata.deviceType = DEVICE_WIFI;
                    abortdata.scanType = AUTO;
                    strcpy(abortdata.cmd,"VITA_DM_Scan_Abort");
                    
                    //bAbortStatus = true;
                    VITA_DM_Abort(abortdata);
                    
                    
                }
                autoScanCount++;
            }
            else
            {
                LOG_PRINT("########## ************ VITA_DM_TESTCASE_8 : FAILED ************ ##########\n");
                testcase = 0;
                
                //bAbortStatus = true;
                DM_Scan_cmd_data abortdata;
                abortdata.deviceType = DEVICE_WIFI;
                abortdata.scanType = AUTO;
                strcpy(abortdata.cmd,"VITA_DM_Scan_Abort");
                
                //bAbortStatus = true;
                VITA_DM_Abort(abortdata);
            }
        }
    }
    else if(testcase == 14)
    {
        if(bt_response == VITA_DM_WIFI_SCAN_STORE_FAIL)
        {
            LOG_PRINT("########## ************ VITA_DM_TESTCASE_14 : PASSED ************ ##########\n");
            testcase = 0;
            
        }
        else
        {
            LOG_PRINT("########## ************ VITA_DM_TESTCASE_14 : FAILED ************ ##########\n");
            testcase = 0;
            
        }
    }
    else if(testcase == 19 && strcmp(scan_cmd, "VITA_DM_ScanAbort") == 0)
    {
        if(bt_response == VITA_DM_WIFI_SCAN_ABORTED)
        {
            LOG_PRINT("########## ************ VITA_DM_TESTCASE_19 : PASSED ************ ##########\n");
            testcase = 0;
            
            
        }
        else
        {
            LOG_PRINT("########## ************ VITA_DM_TESTCASE_19 : FAILED ************ ##########\n");
            testcase = 0;
            
        }
    }
    else if(testcase == 22 && scanType == MANUAL)
    {
        if(bt_response == VITA_DM_WIFI_SCAN_PROGRESS)
        {
            LOG_PRINT("########## ************ VITA_DM_TESTCASE_22 : PASSED ************ ##########\n");
            testcase = 0;
            
            
        }
        else
        {
            LOG_PRINT("########## ************ VITA_DM_TESTCASE_22 : FAILED ************ ##########\n");
            testcase = 0;
            
        }
        DM_Scan_cmd_data abortdata;
        abortdata.deviceType = DEVICE_WIFI;
        abortdata.scanType = AUTO;
        strcpy(abortdata.cmd,"VITA_DM_Scan_Abort");
        
        //bAbortStatus = true;
        VITA_DM_Abort(abortdata);
    }
}

void runInit()
{
    DM_Init_Info_t dminfo;
    dminfo.BT_VITA_Filter.DM_BT_DSM_Filter.bIncludeBtName = 1;
    dminfo.BT_VITA_Filter.DM_BT_DSM_Filter.bIncludeBtDeviceID = 1;
    dminfo.BT_VITA_Filter.DM_BT_DSM_Filter.bIncludeBtClass = 1;
    
    dminfo.WIFI_VITA_Filter.DM_WIFI_DSM_Filter.bIncludeWifiName = 1;
    dminfo.WIFI_VITA_Filter.DM_WIFI_DSM_Filter.bIncludeWifiSignal = 1;
    dminfo.WIFI_VITA_Filter.DM_WIFI_DSM_Filter.bIncludeWifiDeviceID = 1;
    dminfo.WIFI_VITA_Filter.DM_WIFI_DSM_Filter.bIncludeWifiFrequency = 1;
    
    strcpy(dminfo.DeviceId,"B8:27:EB:39:1A:2F");
    strcpy(dminfo.SM_URL,"http://192.168.4.19:8856/iotpoc/user/addUserDeviceDetails");
    
    VITA_DM_Init(dminfo,dminfo.SM_URL);
}

int main(int argc, char **argv)
{
#ifndef TEST
    DM_Init_Info_t Init_info;
    DM_Open_Info_t Open_info;
    
    char choice = '\0';
    char tempurl[] = "http://192.168.5.199:8856/iotpoc/user/addUserDeviceDetails:return strlen(server_url);return strlen(server_url);";
    
    VITA_DM_Init(Init_info, tempurl);
    hDMHandle = VITA_DM_Open(Open_info);
    
    do
    {
        int result = input();
        resultHandler(result);
        LOG_PRINT("Would you like to do another scan: y/n\n");
        scanf(" %c", &choice);
    }while(choice == 'y');
    
#else
    int testCaseID;
    sscanf (argv[1],"%d",&testCaseID);
    
    LOG_PRINT("RECEIEVED ARGUMENT: %d\n",testCaseID);
    
    switch (testCaseID)
    {
        case 2:
            VITA_DM_TEST_CASE_2();
            while(testcase == 2){};
            break;
        case 6:
            runInit();
            VITA_DM_TEST_CASE_6();
            while(testcase == 6){};
            break;
        case 7:
            runInit();
            VITA_DM_TEST_CASE_7();
            while(testcase == 7){};
            break;
        case 8:
            runInit();
            VITA_DM_TEST_CASE_8();
            while(testcase == 8){};
            break;
        case 10:
            runInit();
            VITA_DM_TEST_CASE_10();
            while(testcase == 10){};
            break;
        case 11:
            runInit();
            VITA_DM_TEST_CASE_11();
            while(testcase == 11){};
            break;
        case 12:
            runInit();
            VITA_DM_TEST_CASE_12();
            while(testcase == 12){};
            break;
        case 14:
            VITA_DM_TEST_CASE_14();
            while(testcase == 14){};
            break;
        case 15:
            VITA_DM_TEST_CASE_15();
            while(testcase == 15){};
            break;
        case 19:
            runInit();
            VITA_DM_TEST_CASE_19();
            while(testcase == 19){};
            sleep(SLEEP_TIME_TO_TEST);
            break;
        case 20:
            runInit();
            VITA_DM_TEST_CASE_20();
            while(testcase == 20){};
            break;
        case 22:
            runInit();
            VITA_DM_TEST_CASE_22();
            while(testcase == 22){};
            break;
        case 23:
            runInit();
            VITA_DM_TEST_CASE_23();
            while(testcase == 23){};
            break;
        default:
            LOG_PRINT("Invalid option. Please try again.\n");
    }
    
    
    /*VITA_DM_TEST_CASE_2();
     while(testcase == 2){};
     sleep(SLEEP_TIME_TO_TEST);
     
     VITA_DM_TEST_CASE_6();
     while(testcase == 6){};
     sleep(SLEEP_TIME_TO_TEST);
     
     VITA_DM_TEST_CASE_7();
     while(testcase == 7){};
     sleep(SLEEP_TIME_TO_TEST);
     
     VITA_DM_TEST_CASE_8();
     while(testcase == 8){};
     sleep(SLEEP_TIME_TO_TEST);
     
     
     VITA_DM_TEST_CASE_10();
     while(testcase == 10){};
     sleep(SLEEP_TIME_TO_TEST);
     
     VITA_DM_TEST_CASE_11();
     while(testcase == 11){};
     sleep(SLEEP_TIME_TO_TEST);
     
     VITA_DM_TEST_CASE_12();
     while(testcase == 12){};
     sleep(SLEEP_TIME_TO_TEST);
     
     
     VITA_DM_TEST_CASE_14();
     while(testcase == 14){};
     sleep(SLEEP_TIME_TO_TEST);
     
     VITA_DM_TEST_CASE_15();
     while(testcase == 15){};
     sleep(SLEEP_TIME_TO_TEST);
     
     VITA_DM_TEST_CASE_19();
     while(testcase == 19){};
     sleep(SLEEP_TIME_TO_TEST);
     
     VITA_DM_TEST_CASE_20();
     while(testcase == 20){};
     sleep(SLEEP_TIME_TO_TEST);*/
    
#endif
    return 0;
}
