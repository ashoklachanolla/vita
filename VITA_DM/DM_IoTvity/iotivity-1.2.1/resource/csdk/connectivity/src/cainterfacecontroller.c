/******************************************************************
 *
 * Copyright 2014 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "logger.h"
#include "oic_malloc.h"
#include "caadapterutils.h"
#include "canetworkconfigurator.h"
#include "cainterfacecontroller.h"
#include "caedradapter.h"
#include "caleadapter.h"
#include "canfcadapter.h"
#include "caremotehandler.h"
#include "cathreadpool.h"
#include "caipadapter.h"
#include "cainterface.h"
#include <coap/utlist.h>

#ifdef RA_ADAPTER
#include "caraadapter.h"
#endif

#ifdef TCP_ADAPTER
#include "catcpadapter.h"
#endif

#define TAG "OIC_CA_INF_CTR"

#define CA_MEMORY_ALLOC_CHECK(arg) {if (arg == NULL) \
    {OIC_LOG(ERROR, TAG, "memory error");goto memory_error_exit;} }


extern key_value_pair_t1 *testGetdata;
/**************************************modified S***********************************************/
/***************************************/
/* NON- IoTivity Initial Frame work */
/***************************************/
#define RESOURCE_POSITION_START_INDEX   358
#define RECEIVED_PACKET_LENGTH          451
extern unsigned char ucClientMCdata[];
extern uint32_t unCMCdataLen;

unsigned char clientReceivedPacket[] = {0x58, 0x45, 0x90, 0x41, 0xb6, 0x58, 0xd8, 0x3e, 0xf1, 0xeb, 0xd2, 0xd1, 0xb3, 0x6f, 0x69, 0x63, 0x03, 0x72, 0x65, 0x73, 0x11, 0x3c, 0xe1, 0xfe, 0xdb, 0xc0, 0xff, 0x81, 0xbf, 0x62, 0x64, 0x69, 0x78, 0x24, 0x62, 0x66, 0x35, 0x33, 0x34, 0x65, 0x38, 0x65, 0x2d, 0x36, 0x35, 0x34, 0x32, 0x2d, 0x34, 0x37, 0x63, 0x61, 0x2d, 0x61, 0x61, 0x33, 0x38, 0x2d, 0x64, 0x38, 0x61, 0x61, 0x62, 0x30, 0x36, 0x64, 0x64, 0x36, 0x64, 0x33, 0x65, 0x6c, 0x69, 0x6e, 0x6b, 0x73, 0x85, 0xa4, 0x64, 0x68, 0x72, 0x65, 0x66, 0x6d, 0x2f, 0x6f, 0x69, 0x63, 0x2f, 0x73, 0x65, 0x63, 0x2f, 0x64, 0x6f, 0x78, 0x6d, 0x62, 0x72, 0x74, 0x81, 0x6a, 0x6f, 0x69, 0x63, 0x2e, 0x72, 0x2e, 0x64, 0x6f, 0x78, 0x6d, 0x62, 0x69, 0x66, 0x81, 0x6f, 0x6f, 0x69, 0x63, 0x2e, 0x69, 0x66, 0x2e, 0x62, 0x61, 0x73, 0x65, 0x6c, 0x69, 0x6e, 0x65, 0x61, 0x70, 0xbf, 0x62, 0x62, 0x6d, 0x01, 0x63, 0x73, 0x65, 0x63, 0xf4, 0xff, 0xa4, 0x64, 0x68, 0x72, 0x65, 0x66, 0x6e, 0x2f, 0x6f, 0x69, 0x63, 0x2f, 0x73, 0x65, 0x63, 0x2f, 0x70, 0x73, 0x74, 0x61, 0x74, 0x62, 0x72, 0x74, 0x81, 0x6b, 0x6f, 0x69, 0x63, 0x2e, 0x72, 0x2e, 0x70, 0x73, 0x74, 0x61, 0x74, 0x62, 0x69, 0x66, 0x81, 0x6f, 0x6f, 0x69, 0x63, 0x2e, 0x69, 0x66, 0x2e, 0x62, 0x61, 0x73, 0x65, 0x6c, 0x69, 0x6e, 0x65, 0x61, 0x70, 0xbf, 0x62, 0x62, 0x6d, 0x01, 0x63, 0x73, 0x65, 0x63, 0xf4, 0xff, 0xa4, 0x64, 0x68, 0x72, 0x65, 0x66, 0x66, 0x2f, 0x6f, 0x69, 0x63, 0x2f, 0x64, 0x62, 0x72, 0x74, 0x81, 0x68, 0x6f, 0x69, 0x63, 0x2e, 0x77, 0x6b, 0x2e, 0x64, 0x62, 0x69, 0x66, 0x82, 0x6f, 0x6f, 0x69, 0x63, 0x2e, 0x69, 0x66, 0x2e, 0x62, 0x61, 0x73, 0x65, 0x6c, 0x69, 0x6e, 0x65, 0x68, 0x6f, 0x69, 0x63, 0x2e, 0x69, 0x66, 0x2e, 0x72, 0x61, 0x70, 0xbf, 0x62, 0x62, 0x6d, 0x01, 0x63, 0x73, 0x65, 0x63, 0xf4, 0xff, 0xa4, 0x64, 0x68, 0x72, 0x65, 0x66, 0x66, 0x2f, 0x6f, 0x69, 0x63, 0x2f, 0x70, 0x62, 0x72, 0x74, 0x81, 0x68, 0x6f, 0x69, 0x63, 0x2e, 0x77, 0x6b, 0x2e, 0x70, 0x62, 0x69, 0x66, 0x82, 0x6f, 0x6f, 0x69, 0x63, 0x2e, 0x69, 0x66, 0x2e, 0x62, 0x61, 0x73, 0x65, 0x6c, 0x69, 0x6e, 0x65, 0x68, 0x6f, 0x69, 0x63, 0x2e, 0x69, 0x66, 0x2e, 0x72, 0x61, 0x70, 0xbf, 0x62, 0x62, 0x6d, 0x01, 0x63, 0x73, 0x65, 0x63, 0xf4, 0xff, 0xa4, 0x64, 0x68, 0x72, 0x65, 0x66, 0x71, 0x66, 0x63, 0x3a, 0x34, 0x64, 0x3a, 0x64, 0x34, 0x3a, 0x33, 0x32, 0x3a, 0x36, 0x31, 0x3a, 0x36, 0x31, 0x62, 0x72, 0x74, 0x82, 0x6a, 0x63, 0x6f, 0x72, 0x65, 0x2e, 0x6c, 0x69, 0x67, 0x68, 0x74, 0x70, 0x63, 0x6f, 0x72, 0x65, 0x2e, 0x62, 0x72, 0x69, 0x67, 0x68, 0x74, 0x6c, 0x69, 0x67, 0x68, 0x74, 0x62, 0x69, 0x66, 0x82, 0x6f, 0x6f, 0x69, 0x63, 0x2e, 0x69, 0x66, 0x2e, 0x62, 0x61, 0x73, 0x65, 0x6c, 0x69, 0x6e, 0x65, 0x69, 0x6f, 0x69, 0x63, 0x2e, 0x69, 0x66, 0x2e, 0x6c, 0x6c, 0x61, 0x70, 0xbf, 0x62, 0x62, 0x6d, 0x03, 0x63, 0x73, 0x65, 0x63, 0xf4, 0xff, 0xff};

/************************************modified E*************************************************/


static CAConnectivityHandler_t *g_adapterHandler = NULL;

static uint32_t g_numberOfAdapters = 0;

static CANetworkPacketReceivedCallback g_networkPacketReceivedCallback = NULL;

static CAErrorHandleCallback g_errorHandleCallback = NULL;

static struct CANetworkCallback_t * g_networkChangeCallbackList = NULL;

/**
 * network callback structure is handling
 * for adapter state changed and connection state changed event.
 */
typedef struct CANetworkCallback_t {

    /** Linked list; for multiple callback list.*/
    struct CANetworkCallback_t * next;

    /** Adapter state changed event callback. */
    CAAdapterStateChangedCB adapter;

    /** Connection state changed event callback. */
    CAConnectionStateChangedCB conn;

} CANetworkCallback_t;

static int CAGetAdapterIndex(CATransportAdapter_t cType)
{
    for (uint32_t index=0 ; index < g_numberOfAdapters ; index++)
    {
        if (cType == g_adapterHandler[index].cType )
         {
             return index;
         }
    }
    return -1;
}
extern void non_iotvityadd_resource(char *);

unsigned char getdata[14];
unsigned char duplicateData[100];

CAEndpoint_t *testendpoint = NULL;

extern unsigned char ucClientMCdata[];
extern uint32_t unCMCdataLen;

void recievePacketNotify(char *macAddrs)
{

    printf("Mac addrss %s\n",macAddrs);
    //macAddrs = "non_iotvity/a/Ganesh";
   // non_iotvityAddres(macAddrs);


//    char *tmp = strdup(macAddrs);

//    strcpy(macAddrs,"/non_iotvity-"); //Put str2 or anyother string that you want at the begining
//    strcat(macAddrs, tmp);


    CASecureEndpoint_t endP = {.endpoint = {.adapter = CA_ADAPTER_IP, .flags = CA_DEFAULT_FLAGS}};
    g_networkPacketReceivedCallback(&endP, ucClientMCdata, 26);
    g_networkPacketReceivedCallback(&endP, ucClientMCdata, 26);
    sleep(1);
    printf("ucClientMCdata=========%s\n",ucClientMCdata);
    non_iotvityadd_resource(macAddrs);



}

static void CARegisterCallback(CAConnectivityHandler_t handler)
{
    if (handler.startAdapter == NULL ||
        handler.startListenServer == NULL ||
        handler.stopListenServer == NULL ||
        handler.startDiscoveryServer == NULL ||
        handler.sendData == NULL ||
        handler.sendDataToAll == NULL ||
        handler.GetnetInfo == NULL ||
        handler.readData == NULL ||
        handler.stopAdapter == NULL ||
        handler.terminate == NULL)
    {
        OIC_LOG(ERROR, TAG, "connectivity handler is not enough to be used!");
        return;
    }
    uint32_t numberofAdapters = g_numberOfAdapters + 1;
    CAConnectivityHandler_t *adapterHandler = OICRealloc(g_adapterHandler,
                                   (numberofAdapters) * sizeof(*adapterHandler));
    if (NULL == adapterHandler)
    {
        OIC_LOG(ERROR, TAG, "Memory allocation failed during registration");
        return;
    }
    g_adapterHandler = adapterHandler;
    g_numberOfAdapters = numberofAdapters;
    g_adapterHandler[g_numberOfAdapters-1] = handler;

    OIC_LOG_V(DEBUG, TAG, "%d type adapter, register complete!", handler.cType);
}

/**
 * Add a network callback from caller to the network callback list
 *
 * @param adapterCB  adapter state changed callback
 * @param connCB     connection state changed callback
 *
 * @return
 *     CAResult_t
 */
static CAResult_t AddNetworkStateChangedCallback(CAAdapterStateChangedCB adapterCB,
                                                 CAConnectionStateChangedCB connCB)
{
    OIC_LOG(DEBUG, TAG, "Add NetworkStateChanged Callback");

    if (!adapterCB || !connCB)
    {
        OIC_LOG(ERROR, TAG, "parameter is null");
        return CA_STATUS_INVALID_PARAM;
    }

    CANetworkCallback_t* callback = NULL;
    LL_FOREACH(g_networkChangeCallbackList, callback)
    {
        if (callback && adapterCB == callback->adapter && connCB == callback->conn)
        {
            OIC_LOG(DEBUG, TAG, "this callback is already added");
            return CA_STATUS_OK;
        }
    }

    callback = (CANetworkCallback_t *) OICCalloc(1, sizeof(CANetworkCallback_t));
    if (NULL == callback)
    {
        OIC_LOG(ERROR, TAG, "Memory allocation failed during registration");
        return CA_MEMORY_ALLOC_FAILED;
    }

    callback->adapter = adapterCB;
    callback->conn = connCB;
    LL_APPEND(g_networkChangeCallbackList, callback);
    return CA_STATUS_OK;
}

/**
 * Remove a network callback from the network callback list
 *
 * @param adapterCB  adapter state changed callback
 * @param connCB     connection state changed callback
 *
 * @return
 *     CAResult_t
 */
static CAResult_t RemoveNetworkStateChangedCallback(CAAdapterStateChangedCB adapterCB,
                                                    CAConnectionStateChangedCB connCB)
{
    OIC_LOG(DEBUG, TAG, "Remove NetworkStateChanged Callback");

    CANetworkCallback_t* callback = NULL;
    LL_FOREACH(g_networkChangeCallbackList, callback)
    {
        if (callback && adapterCB == callback->adapter && connCB == callback->conn)
        {
            OIC_LOG(DEBUG, TAG, "remove specific callback");
            LL_DELETE(g_networkChangeCallbackList, callback);
            OICFree(callback);
            return CA_STATUS_OK;
        }
    }
    return CA_STATUS_OK;
}

/**
 * Remove all network callback from the network callback list
 */
static void RemoveAllNetworkStateChangedCallback()
{
    OIC_LOG(DEBUG, TAG, "Remove All NetworkStateChanged Callback");

    CANetworkCallback_t* callback = NULL;
    LL_FOREACH(g_networkChangeCallbackList, callback)
    {
        OIC_LOG(DEBUG, TAG, "remove all callbacks");
        LL_DELETE(g_networkChangeCallbackList, callback);
        OICFree(callback);
    }
}

#ifdef RA_ADAPTER
CAResult_t CASetAdapterRAInfo(const CARAInfo_t *caraInfo)
{
    return CASetRAInfo(caraInfo);
}
#endif

static void CAReceivedPacketCallback(const CASecureEndpoint_t *sep,
                                     const void *data, uint32_t dataLen)
{
    if (g_networkPacketReceivedCallback != NULL)
    {
        g_networkPacketReceivedCallback(sep, data, dataLen);
    }
    else
    {
        OIC_LOG(ERROR, TAG, "network packet received callback is NULL!");
    }
}

/************************************modified S*************************************************/
//void recievePacketNotify(char *macAddrs)
//{

//    CASecureEndpoint_t endP = {.endpoint = {.adapter = CA_ADAPTER_IP, .flags = CA_DEFAULT_FLAGS}};

//    g_networkPacketReceivedCallback(&endP, ucClientMCdata, 26);
//    g_networkPacketReceivedCallback(&endP, ucClientMCdata, 26);
//    framePacket(macAddrs+1,clientReceivedPacket,ucClientMCdata);
//    g_networkPacketReceivedCallback(&endP, clientReceivedPacket, RECEIVED_PACKET_LENGTH);

//}

//void framePacket (char *macAddrs,unsigned char *data, unsigned char *header)
//{

//    for(int i= 0; i < 26 ;i ++)
//    {

//        data[i] = header[i];
//    }
//    data[1] = 0x45;
//    data[25] = 0xdb;

//    for(int i = 0;i < 17;i++)
//    {
//        data[RESOURCE_POSITION_START_INDEX+i] = macAddrs[i];
//    }

//}
/************************************modified E*************************************************/

static void CAAdapterChangedCallback(CATransportAdapter_t adapter, CANetworkStatus_t status)
{
    // Call the callback.
    CANetworkCallback_t* callback  = NULL;
    LL_FOREACH(g_networkChangeCallbackList, callback)
    {
        if (callback && callback->adapter)
        {
            if (CA_INTERFACE_UP == status)
            {
                callback->adapter(adapter, true);
            }
            else if (CA_INTERFACE_DOWN == status)
            {
                callback->adapter(adapter, false);
            }
        }
    }
    OIC_LOG_V(DEBUG, TAG, "[%d] adapter status is changed to [%d]", adapter, status);
}

#if defined(TCP_ADAPTER) || defined(EDR_ADAPTER) || defined(LE_ADAPTER)
static void CAConnectionChangedCallback(const CAEndpoint_t *info, bool isConnected)
{
    // Call the callback.
    CANetworkCallback_t* callback = NULL;
    LL_FOREACH(g_networkChangeCallbackList, callback)
    {
        if (callback && callback->conn)
        {
            callback->conn(info, isConnected);
        }
    }
    OIC_LOG_V(DEBUG, TAG, "[%s] connection status is changed to [%d]", info->addr, isConnected);
}
#endif

static void CAAdapterErrorHandleCallback(const CAEndpoint_t *endpoint,
                                         const void *data, uint32_t dataLen,
                                         CAResult_t result)
{
    OIC_LOG(DEBUG, TAG, "received error from adapter in interfacecontroller");

    // Call the callback.
    if (g_errorHandleCallback != NULL)
    {
        g_errorHandleCallback(endpoint, data, dataLen, result);
    }
}

void CAInitializeAdapters(ca_thread_pool_t handle)
{
    OIC_LOG(DEBUG, TAG, "initialize adapters..");

    // Initialize adapters and register callback.
#ifdef IP_ADAPTER
    CAInitializeIP(CARegisterCallback, CAReceivedPacketCallback, CAAdapterChangedCallback,
                   CAAdapterErrorHandleCallback, handle);
#endif /* IP_ADAPTER */

#ifdef EDR_ADAPTER
    CAInitializeEDR(CARegisterCallback, CAReceivedPacketCallback, CAAdapterChangedCallback,
                    CAConnectionChangedCallback, CAAdapterErrorHandleCallback, handle);
#endif /* EDR_ADAPTER */

#ifdef LE_ADAPTER
    CAInitializeLE(CARegisterCallback, CAReceivedPacketCallback, CAAdapterChangedCallback,
                   CAConnectionChangedCallback, CAAdapterErrorHandleCallback, handle);
#endif /* LE_ADAPTER */

#ifdef RA_ADAPTER
    CAInitializeRA(CARegisterCallback, CAReceivedPacketCallback, CAAdapterChangedCallback,
                   handle);
#endif /* RA_ADAPTER */

#ifdef TCP_ADAPTER
    CAInitializeTCP(CARegisterCallback, CAReceivedPacketCallback, CAAdapterChangedCallback,
                    CAConnectionChangedCallback, CAAdapterErrorHandleCallback, handle);
#endif /* TCP_ADAPTER */

#ifdef NFC_ADAPTER
    CAInitializeNFC(CARegisterCallback, CAReceivedPacketCallback, CAAdapterChangedCallback,
                    CAAdapterErrorHandleCallback, handle);
#endif /* NFC_ADAPTER */
}

void CASetPacketReceivedCallback(CANetworkPacketReceivedCallback callback)
{
    OIC_LOG(DEBUG, TAG, "Set Receiver handle callback");

    g_networkPacketReceivedCallback = callback;
}

void CASetNetworkMonitorCallbacks(CAAdapterStateChangedCB adapterCB,
                                  CAConnectionStateChangedCB connCB)
{
    OIC_LOG(DEBUG, TAG, "Set network monitoring callback");
    CAResult_t res = AddNetworkStateChangedCallback(adapterCB, connCB);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "AddNetworkStateChangedCallback has failed");
    }
}

void CASetErrorHandleCallback(CAErrorHandleCallback errorCallback)
{
    OIC_LOG(DEBUG, TAG, "Set error handle callback");
    g_errorHandleCallback = errorCallback;
}

CAResult_t CAStartAdapter(CATransportAdapter_t transportType)
{
    OIC_LOG_V(DEBUG, TAG, "Start the adapter of CAConnectivityType[%d]", transportType);

    int index = CAGetAdapterIndex(transportType);
    if (0 > index)
    {
        OIC_LOG(ERROR, TAG, "unknown connectivity type!");
        return CA_STATUS_FAILED;
    }

    CAResult_t res = CA_STATUS_FAILED;
    if (g_adapterHandler[index].startAdapter != NULL)
    {
        res = g_adapterHandler[index].startAdapter();
    }

    return res;
}

void CAStopAdapter(CATransportAdapter_t transportType)
{
    OIC_LOG_V(DEBUG, TAG, "Stop the adapter of CATransportType[%d]", transportType);

    int index = CAGetAdapterIndex(transportType);
    if (0 > index)
    {
        OIC_LOG(ERROR, TAG, "unknown transport type!");
        return;
    }

    if (g_adapterHandler[index].stopAdapter != NULL)
    {
        g_adapterHandler[index].stopAdapter();
    }
}

CAResult_t CAGetNetworkInfo(CAEndpoint_t **info, uint32_t *size)
{
    if (info == NULL || size == NULL)
    {
        return CA_STATUS_INVALID_PARAM;
    }

    CAEndpoint_t **tempInfo = (CAEndpoint_t**) OICCalloc(g_numberOfAdapters, sizeof(*tempInfo));
    if (!tempInfo)
    {
        OIC_LOG(ERROR, TAG, "Out of memory!");
        return CA_MEMORY_ALLOC_FAILED;
    }
    uint32_t *tempSize =(uint32_t*) OICCalloc(g_numberOfAdapters, sizeof(*tempSize));
    if (!tempSize)
    {
        OIC_LOG(ERROR, TAG, "Out of memory!");
        OICFree(tempInfo);
        return CA_MEMORY_ALLOC_FAILED;
    }

    CAResult_t res = CA_STATUS_FAILED;
    size_t resSize = 0;
    for (uint32_t index = 0; index < g_numberOfAdapters; index++)
    {
        if (g_adapterHandler[index].GetnetInfo != NULL)
        {
            // #1. get information for each adapter
            res = g_adapterHandler[index].GetnetInfo(&tempInfo[index],
                                                     &tempSize[index]);

            // #2. total size
            if (res == CA_STATUS_OK)
            {
                resSize += tempSize[index];
            }

            OIC_LOG_V(DEBUG,
                      TAG,
                      "%" PRIu32 " adapter network info size is %" PRIu32 " res:%d",
                      index,
                      tempSize[index],
                      res);
        }
    }

    OIC_LOG_V(DEBUG, TAG, "network info total size is %zu!", resSize);

    if (resSize == 0)
    {
        OICFree(tempInfo);
        OICFree(tempSize);
        if (res == CA_ADAPTER_NOT_ENABLED || res == CA_NOT_SUPPORTED)
        {
            return res;
        }
        else
        {
            return CA_STATUS_FAILED;
        }
    }

    // #3. add data into result
    // memory allocation
    CAEndpoint_t *resInfo = (CAEndpoint_t *) OICCalloc(resSize, sizeof (*resInfo));
    CA_MEMORY_ALLOC_CHECK(resInfo);

    // #4. save data
    *info = resInfo;
    *size = resSize;

    for (uint32_t index = 0; index < g_numberOfAdapters; index++)
    {
        // check information
        if (tempSize[index] == 0)
        {
            continue;
        }

        memcpy(resInfo,
               tempInfo[index],
               sizeof(*resInfo) * tempSize[index]);

        resInfo += tempSize[index];

        // free adapter data
        OICFree(tempInfo[index]);
        tempInfo[index] = NULL;
    }
    OICFree(tempInfo);
    OICFree(tempSize);

    OIC_LOG(DEBUG, TAG, "each network info save success!");
    return CA_STATUS_OK;

    // memory error label.
memory_error_exit:

    for (uint32_t index = 0; index < g_numberOfAdapters; index++)
    {
        OICFree(tempInfo[index]);
        tempInfo[index] = NULL;
    }
    OICFree(tempInfo);
    OICFree(tempSize);

    return CA_MEMORY_ALLOC_FAILED;
}

CAResult_t CASendUnicastData(const CAEndpoint_t *endpoint, const void *data, uint32_t length,
                             CADataType_t dataType)
{
    if (endpoint == NULL)
    {
        OIC_LOG(DEBUG, TAG, "Invalid endpoint");
        return CA_STATUS_INVALID_PARAM;
    }

    printf("CASendUnicastData\n");


    unsigned char *ucdata;
    ucdata =  (unsigned char *)data;
       for (int i = 0;i<length ; i++)
       {
           printf("0x%.2x(%c) , ",ucdata[i],ucdata[i]);
       }
       printf("\nlength %d \n",length);

    unsigned char noniotvityPrefixStr[12] = "non_iotvity";


    if(ucdata[0] == 0x58 && (strstr(ucdata, noniotvityPrefixStr) != NULL) && length == 191)
    {

        CASecureEndpoint_t endP = {.endpoint = {.adapter = CA_ADAPTER_IP, .flags = CA_DEFAULT_FLAGS}};
        g_networkPacketReceivedCallback(&endP, data , length);

        return CA_STATUS_OK;

    }
    else if(ucdata[0] == 0x48 && ucdata[1] == 0x01 &&  (strstr(ucdata, noniotvityPrefixStr) != NULL))
    {

        //       if(testendpoint == NULL)
        //       {
        //           testendpoint = (CAEndpoint_t *)OICCalloc(1, sizeof(CAEndpoint_t));
        //           memcpy(testendpoint,endpoint,sizeof(CAEndpoint_t));
        //           //testendpoint = endpoint;
        //          // printf("endpoint stored\n");
        //       }

        if(duplicateData == NULL)
        {
            strcpy (duplicateData,ucdata);
        }
        else if(strcmp (duplicateData,ucdata) == 0)
        {
            return CA_SEND_FAILED;
        }
        strcpy (duplicateData,ucdata);


        for(int i= 2;i< 14 ;i++)
        {
            getdata[i] = ucdata[i];
        }

        unsigned char *p = strstr(ucdata, noniotvityPrefixStr);
        printf("char %c \n",p[13]);

        char uriname[100];

        memset(uriname,'\0',100);

        uriname[0] = '/';
        //memcpy(uriname,p,31);

        memcpy(&uriname[1],p,29);
        //memcpy(&uriname[1],p,31); If mac id with ' '


        printf("Send data\n");
        //JSONObj *getjsonData = NULL;


        CASecureEndpoint_t endP = {.endpoint = {.adapter = CA_ADAPTER_IP, .flags = CA_DEFAULT_FLAGS}};
        g_networkPacketReceivedCallback(&endP, ucClientMCdata, 26);


        key_value_pair_t1 *getjsonData = NULL;
        key_value_pair_t1 *tempJson = NULL;

        //       getjsonData = (key_value_pair_t *)OICCalloc(1, sizeof(key_value_pair_t));

        printf("data in unicast :%s \n",uriname);

        getjsonData = NonIoTivity_Get(uriname);
        tempJson = testGetdata;

        //       printf("going to print json values which are stored in address: %.8x\n", tempJson);

        //       printf("going to print json values which are stored in address key: %s\n", tempJson->key);
        //       printf("going to print json values which are stored in address value: %s\n", tempJson->value);

        //       CASecureEndpoint_t endP = {.endpoint = {.adapter = CA_ADAPTER_IP, .flags = CA_DEFAULT_FLAGS}};

        while(tempJson->pNext != NULL)
        {
            printf("address of data in get method = %s \n",tempJson->key);
            printf("address of data next in get method = %s \n",tempJson->value);
            tempJson = tempJson->pNext;
        }



        sleep(1);
        // if(length < 60  )
        {
            // char *name = "non_iotvity-'00:1A:7D:DA:71:13'";
            non_iotvityGet_res_details(uriname,getjsonData);


        }
        strcpy (duplicateData,ucdata);


        //   return CA_STATUS_OK;

    }
    else if(ucdata[0] == 0x48 && ucdata[1] == 0x03 )
    {


        char uriname[100];
        printf("CASendUnicastData Put Request \n");
        unsigned char noniotvityPrefixStr[12] = "non_iotvity";
        unsigned char *p = strstr(ucdata, noniotvityPrefixStr);
        if(p)
        {
            printf("Found\n");
        }

        memset(uriname,'\0',100);
        uriname[0] = '/';
        memcpy(&uriname[1],p,29);

        printf("CASendUnicastData ==> URI NAME IN UNICAST == %s\n",uriname);
        unsigned char simpledata[length];
        memset(simpledata,'\0',length);
        memcpy(simpledata,ucdata+53,length - 54 );

        int count = length - 54;
        unsigned char *point =  simpledata;
        if(count > 0)
        {
            int keylen = point [0];
            keylen = keylen - 96;
            char name [100];
            memset(name,'\0',100);
            memcpy(name,point+1,keylen);




            int valuelen = point [keylen+1];
            valuelen = valuelen - 96;

            char value [100];
            memset(value,'\0',100);

            memcpy(value,point+keylen+2,valuelen);

            key_value_pair_t1 FillJson;

            strcpy(FillJson.key,name);
            strcpy(FillJson.value,value);
            FillJson.pNext=NULL;

            printf("CASendUnicastData ==> Key is %s \n",name);
            printf("CASendUnicastData ==> Value is %s \n",value);
            NonIoTivity_Put (uriname,&FillJson );//ashok added
            printf("CASendUnicastData ==> After NonIoTivity_Put\n");
        }





        //        printf("Put Request \n");
        //        unsigned char noniotvityPrefixStr[12] = "non_iotvity";

        //        unsigned char *p = strstr(ucdata, noniotvityPrefixStr);
        //        if(p)
        //        {
        //            printf("Found\n");
        //        }
        //        else
        //            return;

        //        char uriname[100];
        //        memset(uriname,'\0',100);
        //        uriname[0] = '/';
        //        memcpy(&uriname[1],p,29);
        //        printf("URI NAME IN UNICAST == %s\n",uriname);


        //        unsigned char payloadData[length];
        //        memset(payloadData,'\0',length);


        //        memcpy(payloadData,ucdata+53,length - 54 );

        //        int location = length - 54;
        //        unsigned char *point =  payloadData;



        //           for (int i = 0;i<50 ; i++)
        //           {
        //               printf("-0x%.2x(%c) , ",payloadData[i],payloadData[i]);
        //           }



        //        static count = 0;
        //        //  if(location >= length-54)
        //        {
        //            int keylen = point [count+0];


        //            printf("location = 0x%.2x == %d\n",point[0],point[0]);
        //            keylen = keylen - 96;
        //            char key [100];
        //            memset(key,'\0',100);
        //            memcpy(key,point+1,keylen);


        //            int valuelen = point [count+keylen+1];

        //            valuelen = valuelen - 96;

        //            char value [100];
        //            memset(value,'\0',100);
        //            memcpy(value,point+valuelen+2,valuelen);



        //            count = point+valuelen+2;
        //            printf("Key === %s,value = %s \n",key,value);

        //        }

    }

    else if(length > 75 &&  (strstr(ucdata, noniotvityPrefixStr) != NULL))
    {
        //printf("Sending get data \n");
        int i;

        for(i= 2;i < 14 ;i++)
        {
            ucdata[i] = getdata[i];
        }

        //printf("Sending get data \n");

        CASecureEndpoint_t endP = {.endpoint = {.adapter = CA_ADAPTER_IP, .flags = CA_DEFAULT_FLAGS}};

        //printf("Sending get data \n");

        g_networkPacketReceivedCallback(&endP, ucdata, length);
        //printf("Sending get data \n");


        return CA_STATUS_OK;

    }



    u_arraylist_t *list = CAGetSelectedNetworkList();
    if (!list)
    {
        OIC_LOG(ERROR, TAG, "No selected network");
        return CA_SEND_FAILED;
    }
    CATransportAdapter_t requestedAdapter = endpoint->adapter ? endpoint->adapter : CA_ALL_ADAPTERS;

    for (uint32_t i = 0; i < u_arraylist_length(list); i++)
    {
        void* ptrType = u_arraylist_get(list, i);

        if (NULL == ptrType)
        {
            continue;
        }

        CATransportAdapter_t connType = *(CATransportAdapter_t *)ptrType;
        if (0 == (connType & requestedAdapter))
        {
            continue;
        }

        int index = CAGetAdapterIndex(connType);

        if (-1 == index)
        {
            OIC_LOG(ERROR, TAG, "unknown transport type!");
            return CA_STATUS_INVALID_PARAM;
        }

        int32_t sentDataLen = 0;

        if (NULL != g_adapterHandler[index].sendData)
        {
            OIC_LOG(DEBUG, TAG, "unicast message to adapter");
            sentDataLen = g_adapterHandler[index].sendData(endpoint, data, length, dataType);
        }

        if (sentDataLen != (int32_t)length)
        {
            OIC_LOG(ERROR, TAG, "error in sending data. Error will be reported in adapter");
#ifdef SINGLE_THREAD
            //in case of single thread, no error handler. Report error immediately
            return CA_SEND_FAILED;
#endif
        }

    }

    return CA_STATUS_OK;
}

CAResult_t CASendMulticastData(const CAEndpoint_t *endpoint, const void *data, uint32_t length,
                               CADataType_t dataType)
{
    u_arraylist_t *list = CAGetSelectedNetworkList();
    if (!list)
    {
        OIC_LOG(DEBUG, TAG, "No selected network");
        return CA_SEND_FAILED;
    }

    CATransportAdapter_t requestedAdapter = endpoint->adapter ? endpoint->adapter : CA_ALL_ADAPTERS;
    size_t selectedLength = u_arraylist_length(list);
    for (size_t i = 0; i < selectedLength; i++)
    {
        void* ptrType = u_arraylist_get(list, i);

        if (NULL == ptrType)
        {
            continue;
        }

        CATransportAdapter_t connType = *(CATransportAdapter_t *)ptrType;
        if (0 == (connType & requestedAdapter))
        {
            continue;
        }

        int index = CAGetAdapterIndex(connType);
        if (0 > index)
        {
            OIC_LOG(ERROR, TAG, "unknown connectivity type!");
            continue;
        }

        uint32_t sentDataLen = 0;

        if (NULL != g_adapterHandler[index].sendDataToAll)
        {
            void *payload = (void *) OICMalloc(length);
            if (!payload)
            {
                OIC_LOG(ERROR, TAG, "Out of memory!");
                return CA_MEMORY_ALLOC_FAILED;
            }
            memcpy(payload, data, length);
            sentDataLen = g_adapterHandler[index].sendDataToAll(endpoint, payload, length, dataType);
            OICFree(payload);
        }

        if (sentDataLen != length)
        {
            OIC_LOG(ERROR, TAG, "sendDataToAll failed! Error will be reported from adapter");
#ifdef SINGLE_THREAD
            //in case of single thread, no error handler. Report error immediately
            return CA_SEND_FAILED;
#endif
        }
    }

    return CA_STATUS_OK;
}

CAResult_t CAStartListeningServerAdapters()
{
    CAResult_t result = CA_STATUS_FAILED;

    u_arraylist_t *list = CAGetSelectedNetworkList();
    if (!list)
    {
        OIC_LOG(ERROR, TAG, "No selected network");
        return result;
    }

    size_t length = u_arraylist_length(list);
    for (size_t i = 0; i < length; i++)
    {
        void* ptrType = u_arraylist_get(list, i);

        if(ptrType == NULL)
        {
            continue;
        }

        CATransportAdapter_t connType = *(CATransportAdapter_t *)ptrType;

        int index = CAGetAdapterIndex(connType);
        if (0 > index)
        {
            OIC_LOG(ERROR, TAG, "unknown connectivity type!");
            continue;
        }

        if (g_adapterHandler[index].startListenServer != NULL)
        {
            const CAResult_t tmp =
                g_adapterHandler[index].startListenServer();

            // Successful listen if at least one adapter started.
            if (CA_STATUS_OK == tmp)
            {
                result = tmp;
            }
        }
    }

    return result;
}

CAResult_t CAStopListeningServerAdapters()
{
    u_arraylist_t *list = CAGetSelectedNetworkList();
    if (!list)
    {
        OIC_LOG(ERROR, TAG, "No selected network");
        return CA_STATUS_FAILED;
    }

    size_t length = u_arraylist_length(list);
    for (size_t i = 0; i < length; i++)
    {
        void* ptrType = u_arraylist_get(list, i);
        if(ptrType == NULL)
        {
            continue;
        }

        CATransportAdapter_t connType = *(CATransportAdapter_t *)ptrType;

        int index = CAGetAdapterIndex(connType);
        if (0 > index)
        {
            OIC_LOG(ERROR, TAG, "unknown connectivity type!");
            continue;
        }

        if (g_adapterHandler[index].stopListenServer != NULL)
        {
            g_adapterHandler[index].stopListenServer();
        }
    }

    return CA_STATUS_OK;
}

CAResult_t CAStartDiscoveryServerAdapters()
{
    CAResult_t result = CA_STATUS_FAILED;

    u_arraylist_t *list = CAGetSelectedNetworkList();

    if (!list)
    {
        OIC_LOG(ERROR, TAG, "No selected network");
        return result;
    }

    size_t length = u_arraylist_length(list);
    for (size_t i = 0; i < length; i++)
    {
        void* ptrType = u_arraylist_get(list, i);

        if(ptrType == NULL)
        {
            continue;
        }

        CATransportAdapter_t connType = *(CATransportAdapter_t *)ptrType;

        int index = CAGetAdapterIndex(connType);
        if (0 > index)
        {
            OIC_LOG(DEBUG, TAG, "unknown connectivity type!");
            continue;
        }

        if (g_adapterHandler[index].startDiscoveryServer != NULL)
        {
            const CAResult_t tmp =
                g_adapterHandler[index].startDiscoveryServer();

            // Successful discovery if at least one adapter started.
            if (CA_STATUS_OK == tmp)
            {
                result = tmp;
            }
        }
    }

    return result;
}

void CATerminateAdapters()
{
    for (uint32_t index = 0; index < g_numberOfAdapters; index++)
    {
        if (g_adapterHandler[index].terminate != NULL)
        {
            g_adapterHandler[index].terminate();
        }
    }

    OICFree(g_adapterHandler);
    g_adapterHandler = NULL;
    g_numberOfAdapters = 0;
}

#ifdef SINGLE_THREAD
CAResult_t CAReadData()
{
    u_arraylist_t *list = CAGetSelectedNetworkList();

    if (!list)
    {
        return CA_STATUS_FAILED;
    }

    uint8_t i = 0;
    for (i = 0; i < u_arraylist_length(list); i++)
    {
        void *ptrType = u_arraylist_get(list, i);
        if (NULL == ptrType)
        {
            OIC_LOG(ERROR, TAG, "get list fail");
            return CA_STATUS_FAILED;
        }

        CATransportAdapter_t connType = *(CATransportAdapter_t *) ptrType;

        int index = CAGetAdapterIndex(connType);
        if (0 > index)
        {
            OIC_LOG(DEBUG, TAG, "unknown connectivity type!");
            continue;
        }

        if (g_adapterHandler[index].readData != NULL)
        {
            g_adapterHandler[index].readData();
        }
    }

    return CA_STATUS_OK;
}
#endif

