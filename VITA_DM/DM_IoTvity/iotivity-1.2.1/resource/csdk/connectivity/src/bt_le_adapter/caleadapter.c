/* ****************************************************************
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
#include "caleadapter.h"

#include <stdio.h>
#include <stdlib.h>

#include "cafragmentation.h"

#include "caleinterface.h"
#include "cacommon.h"
#include "camutex.h"
#include "caadapterutils.h"
#ifndef SINGLE_THREAD
#include "caqueueingthread.h"
#endif
#if defined(__TIZEN__) || defined(__ANDROID__)
#include "caleserver.h"
#include "caleclient.h"
#endif

#include "oic_malloc.h"
#include "oic_string.h"
#include "caremotehandler.h"
#include <coap/pdu.h>
#include"linux/Non_iotivity.h"


extern recievePacketNotify(char*);
PRINT_MAC_ADDRESS(char* address)
{
   printf("Mac Address:%s\n",address);
   NonIoTivity_AddDevice(address,address);
   //recievePacketNotify(ADDRESS);
}
/*
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/l2cap.h>
*/
/**
 * Logging tag for module name.
 */
#define CALEADAPTER_TAG "OIC_CA_LE_ADAP"

/**
 * Stores information of all the senders.
 *
 * This structure will be used to track and defragment all incoming
 * data packet.
 */
CAResult_t CALEAdapterClientReceivedDataEX(const char *remoteAddress,
                                                const uint8_t *data,
                                                uint32_t dataLength,
                                                uint32_t *sentLength);
typedef struct
{
    uint32_t recvDataLen;
    uint32_t totalDataLen;
    uint8_t *defragData;
    CAEndpoint_t *remoteEndpoint;
 } CABLESenderInfo_t;

typedef enum
{
    ADAPTER_EMPTY = 1,
    ADAPTER_BOTH_CLIENT_SERVER,
    ADAPTER_CLIENT,
    ADAPTER_SERVER
} CABLEAdapter_t;

/**
 * Callback to provide the status of the network change to CA layer.
 */
static CAAdapterChangeCallback g_networkCallback = NULL;

/**
 * Callback to provide the status of the connection change to CA layer.
 */
static CAConnectionChangeCallback g_connectionCallback = NULL;

/**
 * Own port value to identify packet owner. Default port value is 1.
 */
static uint8_t g_localBLESourcePort = 1;

/**
 * bleAddress of the local adapter. Value will be initialized to zero,
 * and will be updated later.
 */
static char g_localBLEAddress[18] = { 0 };

/**
 * Variable to differentiate btw GattServer and GattClient.
 */
static CABLEAdapter_t g_adapterType = ADAPTER_EMPTY;

/**
 * Mutex to synchronize the task to be executed on the GattServer
 * function calls.
 */
static ca_mutex g_bleIsServerMutex = NULL;

/**
 * Mutex to synchronize the callback to be called for the network
 * changes.
 */
static ca_mutex g_bleNetworkCbMutex = NULL;

/**
 * Mutex to synchronize the updates of the local LE address of the
 * adapter.
 */
static ca_mutex g_bleLocalAddressMutex = NULL;

/**
 * Reference to thread pool.
 */
static ca_thread_pool_t g_bleAdapterThreadPool = NULL;

/**
 * Mutex to synchronize the task to be pushed to thread pool.
 */
static ca_mutex g_bleAdapterThreadPoolMutex = NULL;

/**
 * Mutex to synchronize the queing of the data from SenderQueue.
 */
static ca_mutex g_bleClientSendDataMutex = NULL;

/**
 * Mutex to synchronize the queing of the data from ReceiverQueue.
 */
static ca_mutex g_bleReceiveDataMutex = NULL;

/**
 * Mutex to synchronize the queing of the data from SenderQueue.
 */
static ca_mutex g_bleServerSendDataMutex = NULL;

/**
 * Mutex to synchronize the callback to be called for the
 * adapterReqResponse.
 */
static ca_mutex g_bleAdapterReqRespCbMutex = NULL;

/**
 * Callback to be called when network packet received from either
 * GattServer or GattClient.
 */
static CANetworkPacketReceivedCallback g_networkPacketReceivedCallback = NULL;

/**
 * Callback to notify error from the BLE adapter.
 */
static CAErrorHandleCallback g_errorHandler = NULL;

#ifdef SINGLE_THREAD
/**
 * Pointer to defragment received data from single threaded routine.
 */
static CABLESenderInfo_t *g_singleThreadReceiveData = NULL;

/**
 * This function will be associated with the receive for single thread.
 *
 * This function will defragment the received data from sender
 * respectively and will send it up to CA layer. Respective sender's
 * header will provide the length of the data sent.
 *
 * @param[in] data       Actual data received from the remote
 *                       device.
 * @param[in] dataLen    Length of the data received from the
 *                       remote device.
 */
static void CALEDataReceiverHandlerSingleThread(const uint8_t *data,
                                                uint32_t dataLen);

/**
 * This function will be associated with the send for single threaded
 * GattServer.
 *
 * This function will fragment the data to the MTU of the transport
 * and send the data in fragments to the adapters. The function will
 * be blocked until all data is sent out from the adapter.
 *
 * @param[in] data       Data to be transmitted from LE.
 * @param[in] dataLen    Length of the Data being transmitted.
 */
static CAResult_t CALEServerSendDataSingleThread(const uint8_t *data,
                                                 uint32_t dataLen);
#endif

/**
 * Register network change notification callback.
 *
 * @param[in]  netCallback  CAAdapterChangeCallback callback which will
 *                          be set for the change in adapter.
 * @param[in]  connCallback CAConnectionChangeCallback callback which will
 *                          be set for the change in connection.
 *
 * @return  0 on success otherwise a positive error value.
 * @retval  ::CA_STATUS_OK  Successful.
 * @retval  ::CA_STATUS_INVALID_PARAM  Invalid input arguments.
 * @retval  ::CA_STATUS_FAILED Operation failed.
 *
 */
static CAResult_t CALERegisterNetworkNotifications(CAAdapterChangeCallback netCallback,
                                                   CAConnectionChangeCallback connCallback);

/**
 * Set the thread pool handle which is required for spawning new
 * thread.
 *
 * @param[in] handle Thread pool handle which is given by above layer
 *                   for using thread creation task.
 *
 */
static void CASetLEAdapterThreadPoolHandle(ca_thread_pool_t handle);

/**
 * Call the callback to the upper layer when the adapter state gets
 * changed.
 *
 * @param[in] adapter_state New state of the adapter to be notified to
 *                          the upper layer.
 */
static void CALEDeviceStateChangedCb(CAAdapterState_t adapter_state);

/**
 * Call the callback to the upper layer when the device connection state gets
 * changed.
 *
 * @param[in] address      LE address of the device to be notified to the upper layer.
 * @param[in] isConnected  whether connection state is connected or not.
 */
static void CALEConnectionStateChangedCb(CATransportAdapter_t adapter, const char* address,
                                         bool isConnected);

/**
 * Used to initialize all required mutex variable for LE Adapter
 * implementation.
 *
 * @return  0 on success otherwise a positive error value.
 * @retval  ::CA_STATUS_OK  Successful.
 * @retval  ::CA_STATUS_INVALID_PARAM  Invalid input arguments.
 * @retval  ::CA_STATUS_FAILED Operation failed.
 *
 */
static CAResult_t CAInitLEAdapterMutex();

/**
 * Terminate all required mutex variables for LE adapter
 * implementation.
 */
static void CATerminateLEAdapterMutex();

/**
 * Prepares and notify error through error callback.
 */
static void CALEErrorHandler(const char *remoteAddress,
                             const uint8_t *data,
                             uint32_t dataLen,
                             CAResult_t result);

#ifndef SINGLE_THREAD
/**
 * Stop condition of recvhandler.
 */
static bool g_dataBleReceiverHandlerState = false;

/**
 * Sender information.
 */
static u_arraylist_t *g_bleServerSenderInfo = NULL;

static u_arraylist_t *g_bleClientSenderInfo = NULL;

/**
 * Queue to process the outgoing packets from GATTClient.
 */
static CAQueueingThread_t *g_bleClientSendQueueHandle = NULL;

/**
 * Queue to process the incoming packets.
 */
static CAQueueingThread_t *g_bleReceiverQueue = NULL;

/**
 * Queue to process the outgoing packets from GATTServer.
 */
static CAQueueingThread_t *g_bleServerSendQueueHandle = NULL;

/**
 * This function will be associated with the sender queue for
 * GattServer.
 *
 * This function will fragment the data to the MTU of the transport
 * and send the data in fragments to the adapters. The function will
 * be blocked until all data is sent out from the adapter.
 *
 * @param[in] threadData Data pushed to the queue which contains the
 *                       info about RemoteEndpoint and Data.
 */
static void CALEServerSendDataThread(void *threadData);

/**
 * This function will be associated with the sender queue for
 * GattClient.
 *
 * This function will fragment the data to the MTU of the transport
 * and send the data in fragments to the adapters. The function will
 * be blocked until all data is sent out from the adapter.
 *
 * @param[in] threadData Data pushed to the queue which contains the
 *                       info about RemoteEndpoint and Data.
 */
static void CALEClientSendDataThread(void *threadData);

/**
 * This function will be associated with the receiver queue.
 *
 * This function will defragment the received data from each sender
 * respectively and will send it up to CA layer.  Respective sender's
 * header will provide the length of the data sent.
 *
 * @param[in] threadData Data pushed to the queue which contains the
 *                       info about RemoteEndpoint and Data.
 */
static void CALEDataReceiverHandler(void *threadData);

/**
 * This function will stop all queues created for GattServer and
 * GattClient. All four queues will be be stopped with this function
 * invocations.
 */
static void CAStopLEQueues();

/**
 * This function will terminate all queues created for GattServer and
 * GattClient. All four queues will be be terminated with this
 * function invocations.
 */
static void CATerminateLEQueues();

/**
 * This function will initalize the Receiver and Sender queues for
 * GattServer. This function will in turn call the functions
 * CAInitBleServerReceiverQueue() and CAInitBleServerSenderQueue() to
 * initialize the queues.
 *
 * @return ::CA_STATUS_OK or Appropriate error code.
 * @retval ::CA_STATUS_OK  Successful.
 * @retval ::CA_STATUS_INVALID_PARAM  Invalid input arguments.
 * @retval ::CA_STATUS_FAILED Operation failed.
 */
static CAResult_t CAInitLEServerQueues();

/**
 * This function will initalize the Receiver and Sender queues for
 * GattClient. This function will inturn call the functions
 * CAInitBleClientReceiverQueue() and CAInitBleClientSenderQueue() to
 * initialize the queues.
 *
 * @return ::CA_STATUS_OK or Appropriate error code.
 * @retval ::CA_STATUS_OK  Successful.
 * @retval ::CA_STATUS_INVALID_PARAM  Invalid input arguments.
 * @retval ::CA_STATUS_FAILED Operation failed.
 *
 */
static CAResult_t CAInitLEClientQueues();

/**
 * This function will initalize the Receiver queue for
 * GattServer. This will initialize the queue to process the function
 * CABLEServerSendDataThread() when ever the task is added to this
 * queue.
 *
 * @return ::CA_STATUS_OK or Appropriate error code.
 * @retval ::CA_STATUS_OK  Successful.
 * @retval ::CA_STATUS_INVALID_PARAM  Invalid input arguments.
 * @retval ::CA_STATUS_FAILED Operation failed.
 */
static CAResult_t CAInitLEServerSenderQueue();

/**
 * This function will initalize the Receiver queue for
 * GattClient. This will initialize the queue to process the function
 * CABLEClientSendDataThread() when ever the task is added to this
 * queue.
 *
 * @return ::CA_STATUS_OK or Appropriate error code.
 * @retval ::CA_STATUS_OK  Successful.
 * @retval ::CA_STATUS_INVALID_PARAM  Invalid input arguments.
 * @retval ::CA_STATUS_FAILED Operation failed.
 */
static CAResult_t CAInitLEClientSenderQueue();

/**
 * This function will initialize the Receiver queue for
 * LEAdapter. This will initialize the queue to process the function
 * CABLEDataReceiverHandler() when ever the task is added to this
 * queue.
 *
 * @return ::CA_STATUS_OK or Appropriate error code
 * @retval ::CA_STATUS_OK  Successful
 * @retval ::CA_STATUS_INVALID_PARAM  Invalid input arguments
 * @retval ::CA_STATUS_FAILED Operation failed
 *
 */
static CAResult_t CAInitLEReceiverQueue();

/**
 * This function will create the Data required to send it in the
 * queue.
 *
 * @param[in] remoteEndpoint Remote endpoint information of the
 *                           server.
 * @param[in] data           Data to be transmitted from LE.
 * @param[in] dataLength     Length of the Data being transmitted.
 *
 * @return ::CA_STATUS_OK or Appropriate error code.
 * @retval ::CA_STATUS_OK  Successful.
 * @retval ::CA_STATUS_INVALID_PARAM  Invalid input arguments.
 * @retval ::CA_STATUS_FAILED Operation failed.
 */
static CALEData_t *CACreateLEData(const CAEndpoint_t *remoteEndpoint,
                                  const uint8_t *data,
                                  uint32_t dataLength,
                                  u_arraylist_t *senderInfo);

/**
 * Used to free the BLE information stored in the sender/receiver
 * queues.
 *
 * @param[in] bleData Information for a particular data segment.
 */
static void CAFreeLEData(CALEData_t *bleData);

/**
 * Free data.
 */
static void CALEDataDestroyer(void *data, uint32_t size);

#ifndef SINGLE_THREAD
/**
 * remove request or response data of send queue.
 *
 * @param[in] queueHandle    queue to process the outgoing packets.
 * @param[in] mutex          mutex related to sender for client / server.
 * @param[in] address        target address to remove data in queue.
 */
static void CALERemoveSendQueueData(CAQueueingThread_t *queueHandle,
                                    ca_mutex mutex,
                                    const char* address);

/**
 * remove all received data of data list from receive queue.
 *
 * @param[in] dataInfoList   received data list to remove for client / server.
 * @param[in] address        target address to remove data in queue.
 */
static void CALERemoveReceiveQueueData(u_arraylist_t *dataInfoList,
                                       const char* address);

/**
 * get received data info and positioned index from the received data list
 * for client / server which is matched same leAddress and port.
 *
 * @param[in]  leAddress       target address to get serderInfo.
 * @param[in]  port            target port to get serderInfo.
 * @param[in]  senderInfoList  received data list for client / server.
 * @param[out] senderInfo      Pointer to contain matched(leAddress and port)
 *                             received data info.
 * @param[out] senderIndex     Pointer to contain matched(leAddress and port)
 *                             received data info index.
 */
static CAResult_t CALEGetSenderInfo(const char *leAddress,
                                    const uint16_t port,
                                    u_arraylist_t *senderInfoList,
                                    CABLESenderInfo_t **senderInfo,
                                    uint32_t *senderIndex);

/**
 * get ports related to remote address. It is need because multi application
 * can have more than 2 senderInfo using same BLE address. So before remove
 * receive queue data, should get port list from sender Info.
 *
 * @param[in]  leAddress       target address to get port in serderInfo.
 * @param[in]  senderInfoList  received data list to remove for client / server.
 * @param[out] portList        target port list related to leAddress.
 */
static CAResult_t CALEGetPortsFromSenderInfo(const char *leAddress,
                                            u_arraylist_t *senderInfoList,
                                            u_arraylist_t *portList);
#endif
//****************VITA_DM_BLE_IMPLEMENTATION***************
#include"IOTivity.c"

#if 0
int connect_status=0;
int fd;
pthread_t tid;
static bool verbose = true;
#define BT_ATT_OP_MTU_REQ			0x02
#define BT_ATT_OP_WRITE_REQ			0x12
#define ATT_CID     					4

typedef struct BLE_FILE_DESCRIPTOR
{
    int fd;
    char address[18];
}BLE_DEVICES;

int count_ble=0;
BLE_DEVICES ble_devices[10];
BLE_DEVICES * ble_pointer=ble_devices;
int update_status(int value)
{

    connect_status = value;

}
static int l2cap_le_att_connect(bdaddr_t *src, bdaddr_t *dst, uint8_t dst_type,
                                    int sec)
{
    printf("*******************%s\n",__func__);
    int sock;
    struct sockaddr_l2 srcaddr, dstaddr;
    struct bt_security btsec;

    if (verbose) {
        char srcaddr_str[18], dstaddr_str[18];
        ba2str(src, srcaddr_str);
        ba2str(dst, dstaddr_str);

        printf("btgatt-client: Opening L2CAP LE connection on ATT "
                    "channel:\n\t src: %s\n\tdest: %s\n",
                    srcaddr_str, dstaddr_str);
    }

    sock = socket(PF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
    if (sock < 0)
    {
        perror("Failed to create L2CAP socket");
        return -1;
    }

    /* Set up source address */
    memset(&srcaddr, 0, sizeof(srcaddr));
    srcaddr.l2_family = AF_BLUETOOTH;
    srcaddr.l2_cid = htobs(ATT_CID);
    srcaddr.l2_bdaddr_type = 0;
    bacpy(&srcaddr.l2_bdaddr, src);

    if (bind(sock, (struct sockaddr *)&srcaddr, sizeof(srcaddr)) < 0)
    {
        perror("Failed to bind L2CAP socket");
        close(sock);
        return -1;
    }

    /* Set the security level */
    memset(&btsec, 0, sizeof(btsec));
    btsec.level = sec;
    if (setsockopt(sock, SOL_BLUETOOTH, BT_SECURITY, &btsec,
                            sizeof(btsec)) != 0) {
        fprintf(stderr, "Failed to set L2CAP security level\n");
        close(sock);
        return -1;
    }

    /* Set up destination address */
    memset(&dstaddr, 0, sizeof(dstaddr));
    dstaddr.l2_family = AF_BLUETOOTH;
    dstaddr.l2_cid = htobs(ATT_CID);
    dstaddr.l2_bdaddr_type = dst_type;
    bacpy(&dstaddr.l2_bdaddr, dst);

    printf("Connecting to device...");
    //fflush(stdout);

    if (connect(sock, (struct sockaddr *) &dstaddr, sizeof(dstaddr)) < 0) {
        perror(" Failed to connect");
        close(sock);
        return -1;
    }

    printf("Done\n");

    return sock;
}

void *read_data(void *address_original)
{
    unsigned char rpdu[512];
    int len=0,temp;
    char address[18];
    memcpy(address,(char*)address_original,18);
    int count = 0;
    int i = 0,fd;
    while(1)
    {
        sleep(2);
        for(i = 0;i<count_ble;i++)
            if(strcasecmp(address,ble_devices[i].address) == 0 )
                    fd = ble_devices[i].fd;
         memset(rpdu,0,sizeof(rpdu));
         len = read(fd,rpdu,512);
            if(len != -1)
            {
                if(rpdu[0] == 0x1B)
                {

                            CALEAdapterClientReceivedDataEX(address,(const uint8_t *)rpdu+3,len-3,&temp);
                            printf("Packet Up....\n\n");
                            count=0;
                    }//wpdu
                if(rpdu[0] == 0x13)
                {
                    printf("Response ok\n\n");
                }
            }//len
            else
            {
                sleep(1);
//                count++;
//                if(count == 5)
//                    return;
#if 0
                //CONNECT_IOT_DM(address);
//              count++;
//              if(count>3)
//                    break;
#endif
            }
    }//while
    return;
}
void init (char* address)
{
     printf("*******************%s\n",__func__);
     int error;
     error=pthread_create(&tid, NULL,read_data,(void *)address);
     if(error!=0)
      printf("thread creation failed\n");
      return;
}
void mtu_request(int fd)
{
    unsigned char wpdu[5];
    unsigned char rpdu[5];
    unsigned int size=0;

    wpdu[0] = BT_ATT_OP_MTU_REQ;
    wpdu[1] = 0x00;
    wpdu[2] = 0x01;

    size = write(fd,wpdu,3);
    sleep( 1 );
    size = read(fd,rpdu,3);

}
void enable_notification(int fd)
{
    unsigned char wpdu[10];
    unsigned int size=0;
    wpdu[0] = BT_ATT_OP_WRITE_REQ;
    wpdu[1] = 0x10;
    wpdu[2] = 0x00;
    wpdu[3] = 0x01;
    wpdu[4] = 0x00;

    size=VITA_WRITE_REQUEST(fd,wpdu,5);
    sleep(1);
}

void add_to_ble_structure(int FD, char * ADDRESS)
{
    int i = 0;
    for(i = 0 ; i < count_ble ; i++ )
    {
         if(strcasecmp(ble_devices[i].address,ADDRESS)==0)
             ble_devices[i].fd=FD;
             return;
    }
    ble_devices[count_ble].fd = FD;
    memcpy(ble_devices[count_ble].address,ADDRESS,18);
    count_ble++;

}

int CONNECT_IOT_DM(char* dst_address)
{
        int count_try=0;
        int sec = BT_SECURITY_LOW;
        uint8_t dst_type = BDADDR_LE_PUBLIC;
        bdaddr_t src_addr, dst_addr;
        int dev_id = -1;
        if (str2ba(dst_address, &dst_addr) < 0)
        {
         perror("Invalid remote address:\n");
         return false;
                }

        if (dev_id == -1)
            bacpy(&src_addr, BDADDR_ANY);
        else if (hci_devba(dev_id, &src_addr) < 0)
        {
            perror("Adapter not available");
            return false;
        }
        while(1)
        {
        fd = l2cap_le_att_connect(&src_addr, &dst_addr, dst_type, sec);
        if(fd < 0)
        {
            count_try++;
            if(count_try==5)
                return false;
        }
        else
            break;
        }
        mtu_request(fd);
       init(dst_address);
        enable_notification(fd);
        add_to_ble_structure(fd,dst_address);
        return true;
}

//****************VITA_DM_BLE_IMPLEMENTATION END*************
#endif

static CAResult_t CAInitLEServerQueues()
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    ca_mutex_lock(g_bleAdapterThreadPoolMutex);

    CAResult_t result = CAInitLEServerSenderQueue();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "CAInitBleServerSenderQueue failed");
        ca_mutex_unlock(g_bleAdapterThreadPoolMutex);
        return CA_STATUS_FAILED;
    }

    g_bleServerSenderInfo = u_arraylist_create();
    if (!g_bleServerSenderInfo)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "memory allocation failed!");
        ca_mutex_unlock(g_bleAdapterThreadPoolMutex);
        return CA_MEMORY_ALLOC_FAILED;
    }

    result = CAInitLEReceiverQueue();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "CAInitLEReceiverQueue failed");
        u_arraylist_free(&g_bleServerSenderInfo);
        ca_mutex_unlock(g_bleAdapterThreadPoolMutex);
        return CA_STATUS_FAILED;
    }

    g_dataBleReceiverHandlerState = true;

    ca_mutex_unlock(g_bleAdapterThreadPoolMutex);

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

static CAResult_t CAInitLEClientQueues()
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    ca_mutex_lock(g_bleAdapterThreadPoolMutex);

    CAResult_t result = CAInitLEClientSenderQueue();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "CAInitBleClientSenderQueue failed");
        ca_mutex_unlock(g_bleAdapterThreadPoolMutex);
        return CA_STATUS_FAILED;
    }

    g_bleClientSenderInfo = u_arraylist_create();
    if (!g_bleClientSenderInfo)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "memory allocation failed!");
        ca_mutex_unlock(g_bleAdapterThreadPoolMutex);
        return CA_MEMORY_ALLOC_FAILED;
    }

    result = CAInitLEReceiverQueue();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "CAInitLEReceiverQueue failed");
        u_arraylist_free(&g_bleClientSenderInfo);
        ca_mutex_unlock(g_bleAdapterThreadPoolMutex);
        return CA_STATUS_FAILED;
    }

    g_dataBleReceiverHandlerState = true;

    ca_mutex_unlock(g_bleAdapterThreadPoolMutex);

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

static CAResult_t CAInitLEReceiverQueue()
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN - CAInitLEReceiverQueue");
    // Check if the message queue is already initialized
    if (g_bleReceiverQueue)
    {
        OIC_LOG(DEBUG, CALEADAPTER_TAG, "Already queue is initialized!");
        return CA_STATUS_OK;
    }

    // Create recv message queue
    g_bleReceiverQueue = (CAQueueingThread_t *) OICMalloc(sizeof(CAQueueingThread_t));
    if (!g_bleReceiverQueue)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "Memory allocation failed!");
        return CA_MEMORY_ALLOC_FAILED;
    }

    if (CA_STATUS_OK != CAQueueingThreadInitialize(g_bleReceiverQueue,
                                                   g_bleAdapterThreadPool,
                                                   CALEDataReceiverHandler,
                                                   CALEDataDestroyer))
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "Failed to Initialize send queue thread");
        OICFree(g_bleReceiverQueue);
        g_bleReceiverQueue = NULL;
        return CA_STATUS_FAILED;
    }

    if (CA_STATUS_OK != CAQueueingThreadStart(g_bleReceiverQueue))
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "ca_thread_pool_add_task failed ");
        OICFree(g_bleReceiverQueue);
        g_bleReceiverQueue = NULL;
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

static CAResult_t CAInitLEServerSenderQueue()
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN - CAInitLEServerSenderQueue");
    // Check if the message queue is already initialized
    if (g_bleServerSendQueueHandle)
    {
        OIC_LOG(DEBUG, CALEADAPTER_TAG, "Queue is already initialized!");
        return CA_STATUS_OK;
    }

    // Create send message queue
    g_bleServerSendQueueHandle = (CAQueueingThread_t *) OICMalloc(sizeof(CAQueueingThread_t));
    if (!g_bleServerSendQueueHandle)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "Memory allocation failed!");
        return CA_MEMORY_ALLOC_FAILED;
    }

    if (CA_STATUS_OK != CAQueueingThreadInitialize(g_bleServerSendQueueHandle,
                                                   g_bleAdapterThreadPool,
                                                   CALEServerSendDataThread,
                                                   CALEDataDestroyer))
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "Failed to Initialize send queue thread");
        OICFree(g_bleServerSendQueueHandle);
        g_bleServerSendQueueHandle = NULL;
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

static void CALEClearSenderInfoImpl(u_arraylist_t ** list)
{
    const size_t length = u_arraylist_length(*list);
    for (size_t i = 0; i < length; ++i)
    {
        CABLESenderInfo_t * const info =
                (CABLESenderInfo_t *) u_arraylist_get(*list, i);
        if (info)
         {
             OICFree(info->defragData);
             CAFreeEndpoint(info->remoteEndpoint);
             OICFree(info);
         }
    }
    u_arraylist_free(list);
}

static void CALEClearSenderInfo()
{
    CALEClearSenderInfoImpl(&g_bleServerSenderInfo);
    CALEClearSenderInfoImpl(&g_bleClientSenderInfo);
}

static CAResult_t CAInitLEClientSenderQueue()
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN - CAInitLEClientSenderQueue");

    if (g_bleClientSendQueueHandle)
    {
        OIC_LOG(DEBUG, CALEADAPTER_TAG, "Already queue is initialized!");
        return CA_STATUS_OK;
    }

    // Create send message queue
    g_bleClientSendQueueHandle = (CAQueueingThread_t *) OICMalloc(sizeof(CAQueueingThread_t));
    if (!g_bleClientSendQueueHandle)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "Memory allocation failed!");
        return CA_MEMORY_ALLOC_FAILED;
    }

    if (CA_STATUS_OK != CAQueueingThreadInitialize(g_bleClientSendQueueHandle,
                                                   g_bleAdapterThreadPool,
                                                   CALEClientSendDataThread, CALEDataDestroyer))
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "Failed to Initialize send queue thread");
        OICFree(g_bleClientSendQueueHandle);
        g_bleClientSendQueueHandle = NULL;
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT - CAInitLEClientSenderQueue");
    return CA_STATUS_OK;
}

static void CAStopLEQueues()
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN - CAStopLEQueues");

    ca_mutex_lock(g_bleReceiveDataMutex);
    if (NULL != g_bleReceiverQueue)
    {
        CAQueueingThreadStop(g_bleReceiverQueue);
    }
    ca_mutex_unlock(g_bleReceiveDataMutex);

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT - CAStopLEQueues");
}

static void CATerminateLEQueues()
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    CAQueueingThreadDestroy(g_bleClientSendQueueHandle);
    OICFree(g_bleClientSendQueueHandle);
    g_bleClientSendQueueHandle = NULL;

    CAQueueingThreadDestroy(g_bleServerSendQueueHandle);
    OICFree(g_bleServerSendQueueHandle);
    g_bleServerSendQueueHandle = NULL;

    CAQueueingThreadDestroy(g_bleReceiverQueue);
    OICFree(g_bleReceiverQueue);
    g_bleReceiverQueue = NULL;

    CALEClearSenderInfo();

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
}

static CAResult_t CALEGetSenderInfo(const char *leAddress,
                                    const uint16_t port,
                                    u_arraylist_t *senderInfoList,
                                    CABLESenderInfo_t **senderInfo,
                                    uint32_t *senderIndex)
{
    VERIFY_NON_NULL_RET(leAddress,
                        CALEADAPTER_TAG,
                        "NULL BLE address argument",
                        CA_STATUS_INVALID_PARAM);
    VERIFY_NON_NULL_RET(senderIndex,
                        CALEADAPTER_TAG,
                        "NULL index argument",
                        CA_STATUS_INVALID_PARAM);

    const uint32_t listLength = u_arraylist_length(senderInfoList);
    const uint32_t addrLength = strlen(leAddress);
    for (uint32_t index = 0; index < listLength; index++)
    {
        CABLESenderInfo_t *info = (CABLESenderInfo_t *) u_arraylist_get(senderInfoList, index);
        if (!info || !(info->remoteEndpoint))
        {
            continue;
        }

        if (!strncmp(info->remoteEndpoint->addr, leAddress, addrLength))
        {
            if (info->remoteEndpoint->port == port)
            {
                *senderIndex = index;
                if (senderInfo)
                {
                    *senderInfo = info;
                }
                return CA_STATUS_OK;
            }
        }
    }

    return CA_STATUS_FAILED;
}

static void CALEDataReceiverHandler(void *threadData)
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN - CALEDataReceiverHandler");

    ca_mutex_lock(g_bleReceiveDataMutex);

    if (g_dataBleReceiverHandlerState)
    {
        OIC_LOG(DEBUG, CALEADAPTER_TAG, "checking for DE Fragmentation");

        CALEData_t *bleData = (CALEData_t *) threadData;
        if (!bleData)
        {
            OIC_LOG(DEBUG, CALEADAPTER_TAG, "Invalid bleData!");
            ca_mutex_unlock(g_bleReceiveDataMutex);
            return;
        }

        if (!(bleData->senderInfo))
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG, "sender info is not available");
            ca_mutex_unlock(g_bleReceiveDataMutex);
            return;
        }

        if (!(bleData->remoteEndpoint))
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG, "Client RemoteEndPoint NULL!!");
            ca_mutex_unlock(g_bleReceiveDataMutex);
            return;
        }

        CABLESenderInfo_t *senderInfo = NULL;
        uint32_t senderIndex = 0;

        //packet parsing
        CABLEPacketStart_t startFlag = CA_BLE_PACKET_NOT_START;
        CABLEPacketSecure_t secureFlag = CA_BLE_PACKET_NON_SECURE;
        uint16_t sourcePort = 0;
        uint16_t destPort = 0;

        CAParseHeader(bleData->data, &startFlag, &sourcePort, &secureFlag, &destPort);
        OIC_LOG_V(DEBUG, CALEADAPTER_TAG,
                  "header info: startFlag[%X] sourcePort[%d] secureFlag[%X] destPort[%d]",
                  startFlag, sourcePort, secureFlag, destPort);

        if (destPort != g_localBLESourcePort && destPort != CA_BLE_MULTICAST_PORT)
        {
            OIC_LOG_V(ERROR, CALEADAPTER_TAG,
                      "this packet is not valid for this app(port mismatch[mine:%d, packet:%d])",
                      g_localBLESourcePort, destPort);
            ca_mutex_unlock(g_bleReceiveDataMutex);
            return;
        }

        bleData->remoteEndpoint->port = sourcePort;

        if (CA_STATUS_OK != CALEGetSenderInfo(bleData->remoteEndpoint->addr,
                                              bleData->remoteEndpoint->port,
                                              bleData->senderInfo,
                                              &senderInfo, &senderIndex))
        {
            OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "This is a new client [%s:%X]",
                      bleData->remoteEndpoint->addr, bleData->remoteEndpoint->port);
        }
        else
        {
            if (startFlag)
            {
                OIC_LOG(ERROR, CALEADAPTER_TAG,
                        "This packet is start packet but exist senderInfo. Remove senderInfo");
                u_arraylist_remove(bleData->senderInfo, senderIndex);
                OICFree(senderInfo->defragData);
                OICFree(senderInfo);
                senderInfo = NULL;
                senderIndex = 0;
            }
        }

        if (!senderInfo)
        {
            uint32_t totalLength = 0;
            if (startFlag)
            {
                CAParseHeaderPayloadLength(bleData->data, CA_BLE_LENGTH_HEADER_SIZE, &totalLength);
            }
            else
            {
                OIC_LOG(ERROR, CALEADAPTER_TAG, "This packet is wrong packet! ignore.");
                ca_mutex_unlock(g_bleReceiveDataMutex);
                return;
            }

            CABLESenderInfo_t *newSender = OICMalloc(sizeof(CABLESenderInfo_t));
            if (!newSender)
            {
                OIC_LOG(ERROR, CALEADAPTER_TAG, "Memory allocation failed for new sender");
                ca_mutex_unlock(g_bleReceiveDataMutex);
                return;
            }
            newSender->recvDataLen = 0;
            newSender->totalDataLen = 0;
            newSender->defragData = NULL;
            newSender->remoteEndpoint = NULL;

            OIC_LOG(DEBUG, CALEADAPTER_TAG, "Parsing the header");

            newSender->totalDataLen = totalLength;

            if (!(newSender->totalDataLen))
            {
                OIC_LOG(ERROR, CALEADAPTER_TAG, "Total Data Length is parsed as 0!!!");
                OICFree(newSender);
                ca_mutex_unlock(g_bleReceiveDataMutex);
                return;
            }

            size_t dataOnlyLen =
                bleData->dataLen - (CA_BLE_HEADER_SIZE + CA_BLE_LENGTH_HEADER_SIZE);
            OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "Total data to be accumulated [%u] bytes",
                      newSender->totalDataLen);
            OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "data received in the first packet [%zu] bytes",
                      dataOnlyLen);

            newSender->defragData = OICCalloc(newSender->totalDataLen + 1,
                                              sizeof(*newSender->defragData));

            if (NULL == newSender->defragData)
            {
                OIC_LOG(ERROR, CALEADAPTER_TAG, "defragData is NULL!");
                OICFree(newSender);
                ca_mutex_unlock(g_bleReceiveDataMutex);
                return;
            }

            const char *remoteAddress = bleData->remoteEndpoint->addr;
            newSender->remoteEndpoint = CACreateEndpointObject(CA_DEFAULT_FLAGS,
                                                               CA_ADAPTER_GATT_BTLE,
                                                               remoteAddress,
                                                               bleData->remoteEndpoint->port);

            if (NULL == newSender->remoteEndpoint)
            {
                OIC_LOG(ERROR, CALEADAPTER_TAG, "remoteEndpoint is NULL!");
                OICFree(newSender->defragData);
                OICFree(newSender);
                ca_mutex_unlock(g_bleReceiveDataMutex);
                return;
            }

            if (newSender->recvDataLen + dataOnlyLen > newSender->totalDataLen)
            {
                OIC_LOG(ERROR, CALEADAPTER_TAG, "buffer is smaller than received data");
                OICFree(newSender->defragData);
                CAFreeEndpoint(newSender->remoteEndpoint);
                OICFree(newSender);
                ca_mutex_unlock(g_bleReceiveDataMutex);
                return;
            }
            memcpy(newSender->defragData,
                   bleData->data + (CA_BLE_HEADER_SIZE + CA_BLE_LENGTH_HEADER_SIZE),
                   dataOnlyLen);
            newSender->recvDataLen += dataOnlyLen;

            u_arraylist_add(bleData->senderInfo,(void *)newSender);

            //Getting newSender index position in bleSenderInfo array list
            if (CA_STATUS_OK !=
                    CALEGetSenderInfo(newSender->remoteEndpoint->addr,
                                      newSender->remoteEndpoint->port,
                                      bleData->senderInfo,
                                      NULL, &senderIndex))
            {
                OIC_LOG(ERROR, CALEADAPTER_TAG, "Existing sender index not found!!");
                OICFree(newSender->defragData);
                CAFreeEndpoint(newSender->remoteEndpoint);
                OICFree(newSender);
                ca_mutex_unlock(g_bleReceiveDataMutex);
                return;
            }
            senderInfo = newSender;
        }
        else
        {
            size_t dataOnlyLen = bleData->dataLen - CA_BLE_HEADER_SIZE;
            if (senderInfo->recvDataLen + dataOnlyLen > senderInfo->totalDataLen)
            {
                OIC_LOG_V(ERROR, CALEADAPTER_TAG,
                          "Data Length exceeding error!! Receiving [%zu] total length [%u]",
                          senderInfo->recvDataLen + dataOnlyLen, senderInfo->totalDataLen);
                u_arraylist_remove(bleData->senderInfo, senderIndex);
                OICFree(senderInfo->defragData);
                OICFree(senderInfo);
                ca_mutex_unlock(g_bleReceiveDataMutex);
                return;
            }
            OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "Copying the data of length [%zu]",
                      dataOnlyLen);
            memcpy(senderInfo->defragData + senderInfo->recvDataLen,
                   bleData->data + CA_BLE_HEADER_SIZE,
                   dataOnlyLen);
            senderInfo->recvDataLen += dataOnlyLen;
            OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "totalDatalength  [%d] received Datalen [%d]",
                                                senderInfo->totalDataLen, senderInfo->recvDataLen);
        }

        if (senderInfo->totalDataLen == senderInfo->recvDataLen)
        {
            ca_mutex_lock(g_bleAdapterReqRespCbMutex);
            if (NULL == g_networkPacketReceivedCallback)
            {
                OIC_LOG(ERROR, CALEADAPTER_TAG, "gReqRespCallback is NULL!");

                u_arraylist_remove(bleData->senderInfo, senderIndex);
                OICFree(senderInfo->defragData);
                OICFree(senderInfo);
                ca_mutex_unlock(g_bleAdapterReqRespCbMutex);
                ca_mutex_unlock(g_bleReceiveDataMutex);
                return;
            }

            OIC_LOG(DEBUG, CALEADAPTER_TAG, "[CALEDataReceiverHandler] Sending data up !");

            const CASecureEndpoint_t tmp =
                {
                    .endpoint = *senderInfo->remoteEndpoint
                };

            g_networkPacketReceivedCallback(&tmp,
                                            senderInfo->defragData,
                                            senderInfo->recvDataLen);
            ca_mutex_unlock(g_bleAdapterReqRespCbMutex);
            u_arraylist_remove(bleData->senderInfo, senderIndex);
            senderInfo->remoteEndpoint = NULL;
            senderInfo->defragData = NULL;
            OICFree(senderInfo);
        }
    }
    ca_mutex_unlock(g_bleReceiveDataMutex);
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
}

static void CALEServerSendDataThread(void *threadData)
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN - CALEServerSendDataThread");

    CALEData_t * const bleData = (CALEData_t *) threadData;
    if (!bleData)
    {
        OIC_LOG(DEBUG, CALEADAPTER_TAG, "Invalid bledata!");
        return;
    }

    uint32_t midPacketCount = 0;
    size_t remainingLen = 0;
    size_t totalLength = 0;
    CABLEPacketSecure_t secureFlag = CA_BLE_PACKET_NON_SECURE;

    CAResult_t result = CAGenerateVariableForFragmentation(bleData->dataLen,
                                                           &midPacketCount,
                                                           &remainingLen,
                                                           &totalLength);

    if (CA_STATUS_OK != result)
    {
        OIC_LOG_V(ERROR, CALEADAPTER_TAG,
                  "CAGenerateVariableForFragmentation failed, result [%d]", result);
        g_errorHandler(bleData->remoteEndpoint, bleData->data, bleData->dataLen, result);
        return;
    }

    OIC_LOG_V(DEBUG, CALEADAPTER_TAG,
              "Packet info: data size[%d] midPacketCount[%u] remainingLen[%zu] totalLength[%zu]",
              bleData->dataLen, midPacketCount, remainingLen, totalLength);

    OIC_LOG_V(DEBUG,
              CALEADAPTER_TAG,
              "Server total Data length with header is [%zu]",
              totalLength);

    uint8_t dataSegment[CA_SUPPORTED_BLE_MTU_SIZE] = {0};
    uint8_t dataHeader[CA_BLE_HEADER_SIZE] = {0};

    if (NULL != bleData->remoteEndpoint) //Unicast Data
    {
        secureFlag = bleData->remoteEndpoint->flags == CA_SECURE ?
            CA_BLE_PACKET_SECURE : CA_BLE_PACKET_NON_SECURE;

        result = CAGenerateHeader(dataHeader,
                                  CA_BLE_PACKET_START,
                                  g_localBLESourcePort,
                                  secureFlag,
                                  bleData->remoteEndpoint->port);
    }
    else                                //Multicast Data
    {
        result = CAGenerateHeader(dataHeader,
                                  CA_BLE_PACKET_START,
                                  g_localBLESourcePort,
                                  secureFlag,
                                  CA_BLE_MULTICAST_PORT);
    }

    if (CA_STATUS_OK != result)
    {
        OIC_LOG_V(ERROR, CALEADAPTER_TAG,
                  "CAGenerateHeader failed, result [%d]", result);
        g_errorHandler(bleData->remoteEndpoint, bleData->data, bleData->dataLen, result);
        return;
    }

    uint8_t lengthHeader[CA_BLE_LENGTH_HEADER_SIZE] = {0};
    result = CAGenerateHeaderPayloadLength(lengthHeader,
                                           CA_BLE_LENGTH_HEADER_SIZE,
                                           bleData->dataLen);

    if (CA_STATUS_OK != result)
    {
        OIC_LOG_V(ERROR, CALEADAPTER_TAG,
                  "CAGenerateHeaderPayloadLength failed, result [%d]", result);
        g_errorHandler(bleData->remoteEndpoint, bleData->data, bleData->dataLen, result);
        return;
    }

    uint32_t length = 0;
    uint32_t dataLen = 0;
    if (CA_SUPPORTED_BLE_MTU_SIZE > totalLength)
    {
        length = totalLength;
        dataLen = bleData->dataLen;
    }
    else
    {
        length = CA_SUPPORTED_BLE_MTU_SIZE;
        dataLen = CA_SUPPORTED_BLE_MTU_SIZE - CA_BLE_HEADER_SIZE - CA_BLE_LENGTH_HEADER_SIZE;
    }

    result = CAMakeFirstDataSegment(dataSegment,
                                    bleData->data, dataLen,
                                    dataHeader, lengthHeader);

    if (CA_STATUS_OK != result)
    {
        OIC_LOG_V(ERROR, CALEADAPTER_TAG,
                  "Making data segment failed, result [%d]", result);
        g_errorHandler(bleData->remoteEndpoint, bleData->data, bleData->dataLen, result);
        return;
    }

    const uint32_t iter = midPacketCount;
    uint32_t index = 0;

    // Send the first segment with the header.
    if (NULL != bleData->remoteEndpoint) // Sending Unicast Data
    {
        OIC_LOG(DEBUG, CALEADAPTER_TAG, "Server Sending Unicast Data");

        result = CAUpdateCharacteristicsToGattClient(
                    bleData->remoteEndpoint->addr, dataSegment, length);

        if (CA_STATUS_OK != result)
        {
            OIC_LOG_V(ERROR,
                      CALEADAPTER_TAG,
                      "Update characteristics failed, result [%d]",
                      result);

            g_errorHandler(bleData->remoteEndpoint,
                           bleData->data,
                           bleData->dataLen,
                           result);
            return;
        }

        OIC_LOG_V(DEBUG,
                  CALEADAPTER_TAG,
                  "Server Sent data length [%u]",
                  length);

        result = CAGenerateHeader(dataHeader,
                                  CA_BLE_PACKET_NOT_START,
                                  g_localBLESourcePort,
                                  secureFlag,
                                  bleData->remoteEndpoint->port);

        if (CA_STATUS_OK != result)
        {
            OIC_LOG_V(ERROR, CALEADAPTER_TAG,
                      "CAGenerateHeader failed, result [%d]", result);
            g_errorHandler(bleData->remoteEndpoint, bleData->data, bleData->dataLen, result);
            return;
        }

        for (index = 0; index < iter; index++)
        {
            // Send the remaining header.
            OIC_LOG_V(DEBUG,
                      CALEADAPTER_TAG,
                      "Sending the chunk number [%u]",
                      index);

            result = CAMakeRemainDataSegment(dataSegment,
                                             bleData->data,
                                             CA_SUPPORTED_BLE_MTU_SIZE - CA_BLE_HEADER_SIZE,
                                             index,
                                             dataHeader);

            if (CA_STATUS_OK != result)
            {
                OIC_LOG_V(ERROR, CALEADAPTER_TAG,
                            "Making data segment failed, result [%d]", result);
                g_errorHandler(bleData->remoteEndpoint, bleData->data, bleData->dataLen, result);
                return;
            }

            result =
                CAUpdateCharacteristicsToGattClient(
                    bleData->remoteEndpoint->addr,
                    dataSegment,
                    CA_SUPPORTED_BLE_MTU_SIZE);

            if (CA_STATUS_OK != result)
            {
                OIC_LOG_V(ERROR, CALEADAPTER_TAG,
                            "Update characteristics failed, result [%d]", result);
                g_errorHandler(bleData->remoteEndpoint, bleData->data, bleData->dataLen, result);
                return;
            }
            OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "Server Sent data length [%d]",
                                               CA_SUPPORTED_BLE_MTU_SIZE);
        }

        if (remainingLen && (totalLength > CA_SUPPORTED_BLE_MTU_SIZE))
        {
            // send the last segment of the data (Ex: 22 bytes of 622
            // bytes of data when MTU is 200)
            OIC_LOG(DEBUG, CALEADAPTER_TAG, "Sending the last chunk");

            result = CAMakeRemainDataSegment(dataSegment,
                                             bleData->data,
                                             remainingLen,
                                             index,
                                             dataHeader);

            if (CA_STATUS_OK != result)
            {
                OIC_LOG_V(ERROR, CALEADAPTER_TAG,
                            "Making data segment failed, result [%d]", result);
                g_errorHandler(bleData->remoteEndpoint, bleData->data, bleData->dataLen, result);
                return;
            }

            result = CAUpdateCharacteristicsToGattClient(
                         bleData->remoteEndpoint->addr,
                         dataSegment,
                         remainingLen + CA_BLE_HEADER_SIZE);

            if (CA_STATUS_OK != result)
            {
                OIC_LOG_V(ERROR,
                          CALEADAPTER_TAG,
                          "Update characteristics failed, result [%d]",
                          result);
                g_errorHandler(bleData->remoteEndpoint,
                               bleData->data,
                               bleData->dataLen,
                               result);
                return;
            }
            OIC_LOG_V(DEBUG, CALEADAPTER_TAG,
                      "Server Sent data length [%zu]", remainingLen + CA_BLE_HEADER_SIZE);
        }
     }
    else
    {
        OIC_LOG(DEBUG, CALEADAPTER_TAG, "Server Sending Multicast data");
        result = CAUpdateCharacteristicsToAllGattClients(dataSegment, length);
        if (CA_STATUS_OK != result)
        {
            OIC_LOG_V(ERROR, CALEADAPTER_TAG, "Update characteristics failed, result [%d]",
                      result);
            CALEErrorHandler(NULL, bleData->data, bleData->dataLen, result);
            return;
        }
        OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "Server Sent data length [%d]", length);

        result = CAGenerateHeader(dataHeader,
                                  CA_BLE_PACKET_NOT_START,
                                  g_localBLESourcePort,
                                  secureFlag,
                                  CA_BLE_MULTICAST_PORT);

        if (CA_STATUS_OK != result)
        {
            OIC_LOG_V(ERROR, CALEADAPTER_TAG,
                      "CAGenerateHeader failed, result [%d]", result);
            g_errorHandler(bleData->remoteEndpoint, bleData->data, bleData->dataLen, result);
            return;
        }

        for (index = 0; index < iter; index++)
        {
            // Send the remaining header.
            OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "Sending the chunk number [%d]", index);

            result = CAMakeRemainDataSegment(dataSegment,
                                             bleData->data,
                                             CA_SUPPORTED_BLE_MTU_SIZE - CA_BLE_HEADER_SIZE,
                                             index,
                                             dataHeader);

            if (CA_STATUS_OK != result)
            {
                OIC_LOG_V(ERROR, CALEADAPTER_TAG,
                            "Making data segment failed, result [%d]", result);
                g_errorHandler(bleData->remoteEndpoint, bleData->data, bleData->dataLen, result);
                return;
            }

            result = CAUpdateCharacteristicsToAllGattClients(
                         dataSegment,
                         CA_SUPPORTED_BLE_MTU_SIZE);

            if (CA_STATUS_OK != result)
            {
                OIC_LOG_V(ERROR, CALEADAPTER_TAG, "Update characteristics failed, result [%d]",
                          result);
                CALEErrorHandler(NULL, bleData->data, bleData->dataLen, result);
                return;
            }
            OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "Server Sent data length [%u]",
                      CA_SUPPORTED_BLE_MTU_SIZE);
        }

        if (remainingLen && (totalLength > CA_SUPPORTED_BLE_MTU_SIZE))
        {
            // send the last segment of the data
            OIC_LOG(DEBUG, CALEADAPTER_TAG, "Sending the last chunk");

            result = CAMakeRemainDataSegment(dataSegment,
                                             bleData->data,
                                             remainingLen,
                                             index,
                                             dataHeader);

            if (CA_STATUS_OK != result)
            {
                OIC_LOG_V(ERROR, CALEADAPTER_TAG,
                            "Making data segment failed, result [%d]", result);
                g_errorHandler(bleData->remoteEndpoint, bleData->data, bleData->dataLen, result);
                return;
            }

            result = CAUpdateCharacteristicsToAllGattClients(
                         dataSegment,
                         remainingLen + CA_BLE_HEADER_SIZE);

            if (CA_STATUS_OK != result)
            {
                OIC_LOG_V(ERROR, CALEADAPTER_TAG, "Update characteristics failed, result [%d]",
                          result);
                CALEErrorHandler(NULL, bleData->data, bleData->dataLen, result);
                return;
            }
            OIC_LOG_V(DEBUG, CALEADAPTER_TAG,
                      "Server Sent data length [%zu]", remainingLen + CA_BLE_HEADER_SIZE);
        }
    }

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT - CALEServerSendDataThread");
}
//*********************
#if 0
void VITA_WRITE_REQUEST(int file_descriptor,  uint8_t *data,  uint8_t length)
{
    unsigned char rpdu[ 512 ];
    printf("VITA_WRITE_REQUEST ==>fd: %d Length:%d \n",file_descriptor,length);
    uint8_t len;
    int i=0;
    int count=0;
    len = write( file_descriptor, data, length );
    printf("write length %d\n",len);
    char address[18];
    for(i=0;i<count_ble;i++)
            if(file_descriptor == ble_devices[i].fd)
            {
                    memcpy(address,ble_devices[i].address,18);
                    break;
            }
    int count_limit=0;
    while(len != length)
    {
        printf(" Write Failure\n");
        printf("fd %d\n",file_descriptor);
        count_limit++;
        if(count_limit==5)
            break;
#if 0
//        for(i=0;i<count_ble;i++)
//            if(file_descriptor == ble_devices[i].fd)
//            {
//                   printf("valid address %s\n",ble_devices[i].address);
//                   sleep(2);

//                  while(CONNECT_IOT_DM(ble_devices[i].address)==false)
//                       sleep(1);
//                   printf("%s\n",ble_devices[i].address);
//                   changed =1;
#endif
                   if(CONNECT_IOT_DM(address))
                   {
                       for(i=0;i<count_ble;i++)
                               if(strcmp(address,ble_devices[i].address)==0)
                               {
                                       file_descriptor=ble_devices[i].fd;
                                       break;
                               }
                   len = write(file_descriptor, data, length );
                   break;
                        }
    }
    sleep(1);
//    int count_read =0;
//    while( 1 )
//   {
////        printf("in loop\n");
////        memset(rpdu,0,sizeof(rpdu));
//      len =read(file_descriptor,rpdu,512);///May be comment it
//       if(rpdu[0] == 0x13)
//       {
//         printf("response Ok\n");
//            break;
//       }
//       sleep(1);
////        count_read++;

//   }
//   printf("response Ok\n");
    return;
}
//*************************************
//bool check_for_ble_status_flag()
//{
//    if(connect_status == 1)
//            return true;
//    else
//        return false;

//}
#endif

static void CALEClientSendDataThread(void *threadData)
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN - CALEClientSendDataThread");
    while(check_for_ble_status_flag()==false)
    {
        sleep(2);
    }
    CALEData_t *bleData = (CALEData_t *) threadData;
    if (!bleData)
    {
        OIC_LOG(DEBUG, CALEADAPTER_TAG, "Invalid bledata!");
        return;
    }

    uint32_t midPacketCount = 0;
    size_t remainingLen = 0;
    size_t totalLength = 0;
    CABLEPacketSecure_t secureFlag = CA_BLE_PACKET_NON_SECURE;

    CAResult_t result = CAGenerateVariableForFragmentation(bleData->dataLen,
                                                           &midPacketCount,
                                                           &remainingLen,
                                                           &totalLength);

    if (CA_STATUS_OK != result)
    {
        OIC_LOG_V(ERROR, CALEADAPTER_TAG,
                  "CAGenerateVariableForFragmentation failed, result [%d]", result);
        g_errorHandler(bleData->remoteEndpoint, bleData->data, bleData->dataLen, result);
        return;
    }

    OIC_LOG_V(DEBUG, CALEADAPTER_TAG,
              "Packet info: data size[%d] midPacketCount[%u] remainingLen[%zu] totalLength[%zu]",
              bleData->dataLen, midPacketCount, remainingLen, totalLength);

    uint8_t dataSegment[CA_SUPPORTED_BLE_MTU_SIZE] = {0};
    uint8_t dataHeader[CA_BLE_HEADER_SIZE] = {0};

    if (NULL != bleData->remoteEndpoint) //Unicast Data
    {
        secureFlag = bleData->remoteEndpoint->flags == CA_SECURE ?
            CA_BLE_PACKET_SECURE : CA_BLE_PACKET_NON_SECURE;

        result = CAGenerateHeader(dataHeader,
                                  CA_BLE_PACKET_START,
                                  g_localBLESourcePort,
                                  secureFlag,
                                  bleData->remoteEndpoint->port);
    }
    else                                //Multicast Data
    {
        result = CAGenerateHeader(dataHeader,
                                  CA_BLE_PACKET_START,
                                  g_localBLESourcePort,
                                  secureFlag,
                                  CA_BLE_MULTICAST_PORT);
    }

    if (CA_STATUS_OK != result)
    {
        OIC_LOG_V(ERROR, CALEADAPTER_TAG,
                  "CAGenerateHeader failed, result [%d]", result);
        g_errorHandler(bleData->remoteEndpoint, bleData->data, bleData->dataLen, result);
        return;
    }

    uint8_t lengthHeader[CA_BLE_LENGTH_HEADER_SIZE] = {0};
    result = CAGenerateHeaderPayloadLength(lengthHeader,
                                           CA_BLE_LENGTH_HEADER_SIZE,
                                           bleData->dataLen);

    if (CA_STATUS_OK != result)
    {
        OIC_LOG_V(ERROR, CALEADAPTER_TAG,
                  "CAGenerateHeaderPayloadLength failed, result [%d]", result);
        g_errorHandler(bleData->remoteEndpoint, bleData->data, bleData->dataLen, result);
        return;
    }

    uint32_t length = 0;
    uint32_t dataLen = 0;
    if (CA_SUPPORTED_BLE_MTU_SIZE > totalLength)
    {
        length = totalLength;
        dataLen = bleData->dataLen;
    }
    else
    {
        length = CA_SUPPORTED_BLE_MTU_SIZE;
        dataLen = CA_SUPPORTED_BLE_MTU_SIZE - CA_BLE_HEADER_SIZE - CA_BLE_LENGTH_HEADER_SIZE;
    }

    result = CAMakeFirstDataSegment(dataSegment,
                                    bleData->data, dataLen,
                                    dataHeader, lengthHeader);

    if (CA_STATUS_OK != result)
    {
        OIC_LOG_V(ERROR, CALEADAPTER_TAG,
                  "Making data segment failed, result [%d]", result);
        g_errorHandler(bleData->remoteEndpoint, bleData->data, bleData->dataLen, result);
        return;
    }

    const uint32_t iter = midPacketCount;
    uint32_t index = 0;
    if (NULL != bleData->remoteEndpoint) //Sending Unicast Data
    {
        OIC_LOG(DEBUG, CALEADAPTER_TAG, "Sending Unicast Data");
        // Send the first segment with the header.
        result =
            CAUpdateCharacteristicsToGattServer(
                bleData->remoteEndpoint->addr,
                dataSegment,
                length,
                LE_UNICAST,
                0);

        if (CA_STATUS_OK != result)
        {
            OIC_LOG_V(ERROR,
                      CALEADAPTER_TAG,
                      "Update characteristics failed, result [%d]",
                      result);
            g_errorHandler(bleData->remoteEndpoint,
                           bleData->data,
                           bleData->dataLen,
                           result);
            return;
        }

        OIC_LOG_V(DEBUG,
                  CALEADAPTER_TAG,
                  "Client Sent Data length  is [%u]",
                  length);

        result = CAGenerateHeader(dataHeader,
                                  CA_BLE_PACKET_NOT_START,
                                  g_localBLESourcePort,
                                  secureFlag,
                                  bleData->remoteEndpoint->port);

        if (CA_STATUS_OK != result)
        {
            OIC_LOG_V(ERROR, CALEADAPTER_TAG,
                      "CAGenerateHeader failed, result [%d]", result);
            g_errorHandler(bleData->remoteEndpoint, bleData->data, bleData->dataLen, result);
            return;
        }

        for (index = 0; index < iter; index++)
        {
            result = CAMakeRemainDataSegment(dataSegment,
                                             bleData->data,
                                             CA_SUPPORTED_BLE_MTU_SIZE - CA_BLE_HEADER_SIZE,
                                             index,
                                             dataHeader);

            if (CA_STATUS_OK != result)
            {
                OIC_LOG_V(ERROR, CALEADAPTER_TAG,
                            "Making data segment failed, result [%d]", result);
                g_errorHandler(bleData->remoteEndpoint, bleData->data, bleData->dataLen, result);
                return;
            }

            // Send the remaining header.
            result = CAUpdateCharacteristicsToGattServer(
                     bleData->remoteEndpoint->addr,
                     dataSegment,
                     CA_SUPPORTED_BLE_MTU_SIZE,
                     LE_UNICAST, 0);

            if (CA_STATUS_OK != result)
            {
                OIC_LOG_V(ERROR,
                          CALEADAPTER_TAG,
                          "Update characteristics failed, result [%d]",
                          result);
                g_errorHandler(bleData->remoteEndpoint, bleData->data, bleData->dataLen, result);
                return;
            }
            OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "Client Sent Data length  is [%d]",
                                               CA_SUPPORTED_BLE_MTU_SIZE);
        }

        if (remainingLen && (totalLength > CA_SUPPORTED_BLE_MTU_SIZE))
        {
            // send the last segment of the data (Ex: 22 bytes of 622
            // bytes of data when MTU is 200)
            OIC_LOG(DEBUG, CALEADAPTER_TAG, "Sending the last chunk");

            result = CAMakeRemainDataSegment(dataSegment,
                                             bleData->data,
                                             remainingLen,
                                             index,
                                             dataHeader);

            if (CA_STATUS_OK != result)
            {
                OIC_LOG_V(ERROR, CALEADAPTER_TAG,
                            "Making data segment failed, result [%d]", result);
                g_errorHandler(bleData->remoteEndpoint, bleData->data, bleData->dataLen, result);
                return;
            }

            result = CAUpdateCharacteristicsToGattServer(
                     bleData->remoteEndpoint->addr,
                     dataSegment,
                     remainingLen + CA_BLE_HEADER_SIZE,
                     LE_UNICAST, 0);

            if (CA_STATUS_OK != result)
            {
                OIC_LOG_V(ERROR, CALEADAPTER_TAG, "Update characteristics failed, result [%d]",
                                                   result);
                g_errorHandler(bleData->remoteEndpoint, bleData->data, bleData->dataLen, result);
                return;
            }
            OIC_LOG_V(DEBUG, CALEADAPTER_TAG,
                      "Client Sent Data length  is [%zu]", remainingLen + CA_BLE_HEADER_SIZE);
        }
    }
    else
    {
        //Sending Mulitcast Data
        // Send the first segment with the header.
        OIC_LOG(DEBUG, CALEADAPTER_TAG, "Sending Multicast Data");
        result = CAUpdateCharacteristicsToAllGattServers(dataSegment, length);
        if (CA_STATUS_OK != result)
        {
            OIC_LOG_V(ERROR, CALEADAPTER_TAG,
                      "Update characteristics (all) failed, result [%d]", result);
            CALEErrorHandler(NULL, bleData->data, bleData->dataLen, result);
            return ;
        }
        OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "Client Sent Data length  is [%d]", length);

        result = CAGenerateHeader(dataHeader,
                                  CA_BLE_PACKET_NOT_START,
                                  g_localBLESourcePort,
                                  secureFlag,
                                  0);

        if (CA_STATUS_OK != result)
        {
            OIC_LOG_V(ERROR, CALEADAPTER_TAG,
                      "CAGenerateHeader failed, result [%d]", result);
            g_errorHandler(bleData->remoteEndpoint, bleData->data, bleData->dataLen, result);
            return;
        }

        // Send the remaining header.
        for (index = 0; index < iter; index++)
        {
            result = CAMakeRemainDataSegment(dataSegment,
                                             bleData->data,
                                             CA_SUPPORTED_BLE_MTU_SIZE - CA_BLE_HEADER_SIZE,
                                             index,
                                             dataHeader);

            if (CA_STATUS_OK != result)
            {
                OIC_LOG_V(ERROR, CALEADAPTER_TAG,
                            "Making data segment failed, result [%d]", result);
                g_errorHandler(bleData->remoteEndpoint, bleData->data, bleData->dataLen, result);
                return;
            }

            result = CAUpdateCharacteristicsToAllGattServers(
                         dataSegment,
                         CA_SUPPORTED_BLE_MTU_SIZE);

            if (CA_STATUS_OK != result)
            {
                OIC_LOG_V(ERROR, CALEADAPTER_TAG, "Update characteristics failed, result [%d]",
                          result);
                CALEErrorHandler(NULL, bleData->data, bleData->dataLen, result);
                return;
            }
            OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "Client Sent Data length  is [%d]",
                      CA_SUPPORTED_BLE_MTU_SIZE);
        }

        if (remainingLen && (totalLength > CA_SUPPORTED_BLE_MTU_SIZE))
        {
            // send the last segment of the data (Ex: 22 bytes of 622
            // bytes of data when MTU is 200)
            OIC_LOG(DEBUG, CALEADAPTER_TAG, "Sending the last chunk");

            result = CAMakeRemainDataSegment(dataSegment,
                                             bleData->data,
                                             remainingLen,
                                             index,
                                             dataHeader);

            if (CA_STATUS_OK != result)
            {
                OIC_LOG_V(ERROR, CALEADAPTER_TAG,
                            "Making data segment failed, result [%d]", result);
                g_errorHandler(bleData->remoteEndpoint, bleData->data, bleData->dataLen, result);
                return;
            }

            result =
                CAUpdateCharacteristicsToAllGattServers(
                    dataSegment,
                    remainingLen + CA_BLE_HEADER_SIZE);

            if (CA_STATUS_OK != result)
            {
                OIC_LOG_V(ERROR, CALEADAPTER_TAG,
                          "Update characteristics (all) failed, result [%d]", result);
                CALEErrorHandler(NULL, bleData->data, bleData->dataLen, result);
                return;
            }
            OIC_LOG_V(DEBUG, CALEADAPTER_TAG,
                      "Client Sent Data length  is [%zu]", remainingLen + CA_BLE_HEADER_SIZE);
        }
    }

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT - CALEClientSendDataThread");

}

static CALEData_t *CACreateLEData(const CAEndpoint_t *remoteEndpoint,
                                  const uint8_t *data,
                                  uint32_t dataLength,
                                  u_arraylist_t *senderInfo)
{
    CALEData_t * const bleData = OICMalloc(sizeof(CALEData_t));

    if (!bleData)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "Memory allocation failed!");
        return NULL;
    }

    bleData->remoteEndpoint = CACloneEndpoint(remoteEndpoint);
    bleData->data = OICCalloc(dataLength + 1, 1);

    if (NULL == bleData->data)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "Memory allocation failed!");
        CAFreeLEData(bleData);
        return NULL;
    }

    memcpy(bleData->data, data, dataLength);
    bleData->dataLen = dataLength;
    if (senderInfo)
    {
        bleData->senderInfo = senderInfo;
    }

    return bleData;
}

static void CAFreeLEData(CALEData_t *bleData)
{
    VERIFY_NON_NULL_VOID(bleData, CALEADAPTER_TAG, "Param bleData is NULL");

    CAFreeEndpoint(bleData->remoteEndpoint);
    OICFree(bleData->data);
    OICFree(bleData);
}

static void CALEDataDestroyer(void *data, uint32_t size)
{
    if ((size_t)size < sizeof(CALEData_t *))
    {
        OIC_LOG_V(ERROR, CALEADAPTER_TAG,
                  "Destroy data too small %p %d", data, size);
    }
    CALEData_t *ledata = (CALEData_t *) data;

    CAFreeLEData(ledata);
}
#endif

#ifdef SINGLE_THREAD
static void CALEDataReceiverHandlerSingleThread(const uint8_t *data,
                                                uint32_t dataLen)
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    VERIFY_NON_NULL(data, CALEADAPTER_TAG, "Param data is NULL");

    //packet parsing
    CABLEPacketStart_t startFlag = CA_BLE_PACKET_NOT_START;
    CABLEPacketSecure_t secureFlag = CA_BLE_PACKET_NON_SECURE;
    uint16_t sourcePort = 0;
    uint16_t destPort = 0;

    CAParseHeader(data, &startFlag, &sourcePort, &secureFlag, &destPort);
    OIC_LOG_V(DEBUG, CALEADAPTER_TAG,
              "header info: startFlag[%X] sourcePort[%d] secureFlag[%X] destPort[%d]",
              startFlag, sourcePort, secureFlag, destPort);

    if (destPort != g_localBLESourcePort && destPort != CA_BLE_MULTICAST_PORT)
    {
        OIC_LOG_V(ERROR, CALEADAPTER_TAG,
                  "this packet is not valid for this app(port mismatch[mine:%d, packet:%d])",
                  g_localBLESourcePort, destPort);
        return;
    }

    if (startFlag)
    {
        if (g_singleThreadReceiveData)
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG,
                    "This packet is start packet but exist senderInfo. Remove senderInfo");
            OICFree(g_singleThreadReceiveData->defragData);
            OICFree(g_singleThreadReceiveData);
            g_singleThreadReceiveData = NULL;
        }

        uint32_t totalLength = 0;
        CAParseHeaderPayloadLength(data, CA_BLE_LENGTH_HEADER_SIZE, &totalLength);

        g_singleThreadReceiveData = OICMalloc(sizeof(CABLESenderInfo_t));

        if (!g_singleThreadReceiveData)
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG, "Memory allocation failed for new sender");
            return;
        }
        g_singleThreadReceiveData->recvDataLen = 0;
        g_singleThreadReceiveData->totalDataLen = 0;
        g_singleThreadReceiveData->defragData = NULL;
        g_singleThreadReceiveData->remoteEndpoint = NULL;

        OIC_LOG(DEBUG, CALEADAPTER_TAG, "Parsing the header");

        g_singleThreadReceiveData->totalDataLen = totalLength;

        if (!(g_singleThreadReceiveData->totalDataLen))
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG, "Total Data Length is parsed as 0!!!");
            OICFree(g_singleThreadReceiveData);
            return;
        }

        size_t dataOnlyLen = dataLen - (CA_BLE_HEADER_SIZE + CA_BLE_LENGTH_HEADER_SIZE);
        OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "Total data to be accumulated [%u] bytes",
                  g_singleThreadReceiveData->totalDataLen);
        OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "data received in the first packet [%u] bytes",
                  dataOnlyLen);

        g_singleThreadReceiveData->defragData =
            OICCalloc(g_singleThreadReceiveData->totalDataLen + 1,
                    sizeof(*g_singleThreadReceiveData->defragData));

        if (NULL == g_singleThreadReceiveData->defragData)
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG, "defragData is NULL!");
            OICFree(g_singleThreadReceiveData);
            return;
        }

        if (g_singleThreadReceiveData->recvDataLen + dataOnlyLen
                > g_singleThreadReceiveData->totalDataLen)
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG, "buffer is smaller than received data");
            OICFree(g_singleThreadReceiveData->defragData);
            OICFree(g_singleThreadReceiveData);
            return;
        }
        memcpy(g_singleThreadReceiveData->defragData,
               data + (CA_BLE_HEADER_SIZE + CA_BLE_LENGTH_HEADER_SIZE),
               dataOnlyLen);
        g_singleThreadReceiveData->recvDataLen += dataOnlyLen;
    }
    else
    {
        size_t dataOnlyLen = dataLen - CA_BLE_HEADER_SIZE;
        if (g_singleThreadReceiveData->recvDataLen + dataOnlyLen
                > g_singleThreadReceiveData->totalDataLen)
        {
            OIC_LOG_V(ERROR, CALEADAPTER_TAG,
                      "Data Length exceeding error!! Receiving [%d] total length [%d]",
                      g_singleThreadReceiveData->recvDataLen + dataOnlyLen,
                      g_singleThreadReceiveData->totalDataLen);
            OICFree(g_singleThreadReceiveData->defragData);
            OICFree(g_singleThreadReceiveData);
            return;
        }
        OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "Copying the data of length [%d]",
                  dataOnlyLen);
        memcpy(g_singleThreadReceiveData->defragData + g_singleThreadReceiveData->recvDataLen,
               data + CA_BLE_HEADER_SIZE,
               dataOnlyLen);
        g_singleThreadReceiveData->recvDataLen += dataOnlyLen;
        OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "totalDatalength  [%d] received Datalen [%d]",
                g_singleThreadReceiveData->totalDataLen, g_singleThreadReceiveData->recvDataLen);
    }
}

static CAResult_t CALEServerSendDataSingleThread(const uint8_t *data,
                                                 uint32_t dataLen)
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    VERIFY_NON_NULL(data, CALEADAPTER_TAG, "Param data is NULL");

    uint32_t midPacketCount = 0;
    size_t remainingLen = 0;
    size_t totalLength = 0;
    CABLEPacketSecure_t secureFlag = CA_BLE_PACKET_NON_SECURE;

    CAResult_t result = CAGenerateVariableForFragmentation(dataLen,
                                                           &midPacketCount,
                                                           &remainingLen,
                                                           &totalLength);

    if (CA_STATUS_OK != result)
    {
        OIC_LOG_V(ERROR, CALEADAPTER_TAG,
                  "CAGenerateVariableForFragmentation failed, result [%d]", result);
        return result;
    }

    OIC_LOG_V(DEBUG, CALEADAPTER_TAG,
              "Packet info: data size[%d] midPacketCount[%d] remainingLen[%d] totalLength[%d]",
              dataLen, midPacketCount, remainingLen, totalLength);

    OIC_LOG_V(DEBUG,
              CALEADAPTER_TAG,
              "Server total Data length with header is [%u]",
              totalLength);

    uint8_t dataSegment[CA_SUPPORTED_BLE_MTU_SIZE] = {0};
    uint8_t dataHeader[CA_BLE_HEADER_SIZE] = {0};

    result = CAGenerateHeader(dataHeader,
                              CA_BLE_PACKET_START,
                              g_localBLESourcePort,
                              CA_BLE_PACKET_NON_SECURE,
                              CA_BLE_MULTICAST_PORT);

    if (CA_STATUS_OK != result)
    {
        OIC_LOG_V(ERROR, CALEADAPTER_TAG,
                  "CAGenerateHeader failed, result [%d]", result);
        return result;
    }

    uint8_t lengthHeader[CA_BLE_LENGTH_HEADER_SIZE] = {0};
    result = CAGenerateHeaderPayloadLength(lengthHeader,
                                           CA_BLE_LENGTH_HEADER_SIZE,
                                           dataLen);

    if (CA_STATUS_OK != result)
    {
        OIC_LOG_V(ERROR, CALEADAPTER_TAG,
                  "CAGenerateHeaderPayloadLength failed, result [%d]", result);
        return result;
    }

    uint32_t length = 0;
    uint32_t dataOnlyLen = 0;
    if (CA_SUPPORTED_BLE_MTU_SIZE > totalLength)
    {
        length = totalLength;
        dataOnlyLen = dataLen;
    }
    else
    {
        length = CA_SUPPORTED_BLE_MTU_SIZE;
        dataOnlyLen = CA_SUPPORTED_BLE_MTU_SIZE - CA_BLE_HEADER_SIZE - CA_BLE_LENGTH_HEADER_SIZE;
    }

    result = CAMakeFirstDataSegment(dataSegment,
                                    data, dataOnlyLen,
                                    dataHeader, lengthHeader);

    if (CA_STATUS_OK != result)
    {
        OIC_LOG_V(ERROR, CALEADAPTER_TAG,
                  "Making data segment failed, result [%d]", result);
        return result;
    }

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "Server Sending Multicast data");
    result = CAUpdateCharacteristicsToAllGattClients(dataSegment, length);
    if (CA_STATUS_OK != result)
    {
        OIC_LOG_V(ERROR, CALEADAPTER_TAG, "Update characteristics failed, result [%d]",
                result);
        return result;
    }

    CALEDoEvents();

    OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "Server Sent data length [%d]", length);

    result = CAGenerateHeader(dataHeader,
                              CA_BLE_PACKET_NOT_START,
                              g_localBLESourcePort,
                              CA_BLE_PACKET_NON_SECURE,
                              CA_BLE_MULTICAST_PORT);

    if (CA_STATUS_OK != result)
    {
        OIC_LOG_V(ERROR, CALEADAPTER_TAG,
                  "CAGenerateHeader failed, result [%d]", result);
        return result;
    }

    const uint32_t dataLimit = midPacketCount;
    for (uint32_t iter = 0; iter < dataLimit; iter++)
    {
        result = CAMakeRemainDataSegment(dataSegment,
                                         data,
                                         CA_SUPPORTED_BLE_MTU_SIZE - CA_BLE_HEADER_SIZE,
                                         iter,
                                         dataHeader);

        if (CA_STATUS_OK != result)
        {
            OIC_LOG_V(ERROR, CALEADAPTER_TAG,
                    "Making data segment failed, result [%d]", result);
            return result;
        }

        result = CAUpdateCharacteristicsToAllGattClients(
                     dataSegment,
                     CA_SUPPORTED_BLE_MTU_SIZE);

        if (CA_STATUS_OK != result)
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG, "Update characteristics failed");
            return result;
        }

        CALEDoEvents();
    }

    if (remainingLen && (totalLength > CA_SUPPORTED_BLE_MTU_SIZE))
    {
        // send the last segment of the data
        OIC_LOG(DEBUG, CALEADAPTER_TAG, "Sending the last chunk");

        result = CAMakeRemainDataSegment(dataSegment,
                                         data,
                                         remainingLen,
                                         dataLimit,
                                         dataHeader);

        if (CA_STATUS_OK != result)
        {
            OIC_LOG_V(ERROR, CALEADAPTER_TAG,
                    "Making data segment failed, result [%d]", result);
            return result;
        }

        result = CAUpdateCharacteristicsToAllGattClients(
                     dataSegment,
                     remainingLen + CA_BLE_HEADER_SIZE);

        if (CA_STATUS_OK != result)
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG, "Update characteristics failed");
            return result;
        }
        CALEDoEvents();
    }

    return result;
}
#endif

static CAResult_t CAInitLEAdapterMutex()
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN - CAInitLEAdapterMutex");

    if (NULL == g_bleIsServerMutex)
    {
        g_bleIsServerMutex = ca_mutex_new();
        if (NULL == g_bleIsServerMutex)
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG, "ca_mutex_new failed");
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == g_bleNetworkCbMutex)
    {
        g_bleNetworkCbMutex = ca_mutex_new();
        if (NULL == g_bleNetworkCbMutex)
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG, "ca_mutex_new failed");
            CATerminateLEAdapterMutex();
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == g_bleLocalAddressMutex)
    {
        g_bleLocalAddressMutex = ca_mutex_new();
        if (NULL == g_bleLocalAddressMutex)
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG, "ca_mutex_new failed");
            CATerminateLEAdapterMutex();
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == g_bleAdapterThreadPoolMutex)
    {
        g_bleAdapterThreadPoolMutex = ca_mutex_new();
        if (NULL == g_bleAdapterThreadPoolMutex)
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG, "ca_mutex_new failed");
            CATerminateLEAdapterMutex();
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == g_bleClientSendDataMutex)
    {
        g_bleClientSendDataMutex = ca_mutex_new();
        if (NULL == g_bleClientSendDataMutex)
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG, "ca_mutex_new failed");
            CATerminateLEAdapterMutex();
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == g_bleServerSendDataMutex)
    {
        g_bleServerSendDataMutex = ca_mutex_new();
        if (NULL == g_bleServerSendDataMutex)
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG, "ca_mutex_new failed");
            CATerminateLEAdapterMutex();
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == g_bleAdapterReqRespCbMutex)
    {
        g_bleAdapterReqRespCbMutex = ca_mutex_new();
        if (NULL == g_bleAdapterReqRespCbMutex)
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG, "ca_mutex_new failed");
            CATerminateLEAdapterMutex();
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == g_bleReceiveDataMutex)
    {
        g_bleReceiveDataMutex = ca_mutex_new();
        if (NULL == g_bleReceiveDataMutex)
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG, "ca_mutex_new failed");
            return CA_STATUS_FAILED;
        }
    }

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

static void CATerminateLEAdapterMutex()
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN - CATerminateLEAdapterMutex");

    ca_mutex_free(g_bleIsServerMutex);
    g_bleIsServerMutex = NULL;

    ca_mutex_free(g_bleNetworkCbMutex);
    g_bleNetworkCbMutex = NULL;

    ca_mutex_free(g_bleLocalAddressMutex);
    g_bleLocalAddressMutex = NULL;

    ca_mutex_free(g_bleAdapterThreadPoolMutex);
    g_bleAdapterThreadPoolMutex = NULL;

    ca_mutex_free(g_bleClientSendDataMutex);
    g_bleClientSendDataMutex = NULL;

    ca_mutex_free(g_bleServerSendDataMutex);
    g_bleServerSendDataMutex = NULL;

    ca_mutex_free(g_bleAdapterReqRespCbMutex);
    g_bleAdapterReqRespCbMutex = NULL;

    ca_mutex_free(g_bleReceiveDataMutex);
    g_bleReceiveDataMutex = NULL;

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
}

/**
 * Starting LE connectivity adapters.
 *
 * As its peer to peer it does not require to start any servers.
 *
 * @return ::CA_STATUS_OK or Appropriate error code.
 */
static CAResult_t CAStartLE();

/**
 * Start listening server for receiving multicast search requests.
 *
 * Transport Specific Behavior:
 *   LE  Starts GATT Server with prefixed UUID and Characteristics
 *   per OIC Specification.
 * @return  ::CA_STATUS_OK or Appropriate error code.
 */
static CAResult_t CAStartLEListeningServer();

/**
 * Stops listening server from receiving multicast search requests.
 *
 * Transport Specific Behavior:
 *   LE  Starts GATT Server with prefixed UUID and Characteristics
 *   per OIC Specification.
 * @return  ::CA_STATUS_OK or Appropriate error code.
 */
static CAResult_t CAStopLEListeningServer();

/**
 * Sarting discovery of servers for receiving multicast
 * advertisements.
 *
 * Transport Specific Behavior:
 *   LE  Starts GATT Server with prefixed UUID and Characteristics
 *   per OIC Specification.
 *
 * @return ::CA_STATUS_OK or Appropriate error code
 */
static CAResult_t CAStartLEDiscoveryServer();

/**
 * Send data to the endpoint using the adapter connectivity.
 *
 * @param[in] endpoint Remote Endpoint information (like MAC address,
 *                     reference URI and connectivity type) to which
 *                     the unicast data has to be sent.
 * @param[in] data     Data which required to be sent.
 * @param[in] dataLen  Size of data to be sent.
 *
 * @note  dataLen must be > 0.
 *
 * @return The number of bytes sent on the network, or -1 on error.
 */
static int32_t CASendLEUnicastData(const CAEndpoint_t *endpoint,
                                   const void *data,
                                   uint32_t dataLen,
                                   CADataType_t dataType);

/**
 * Send multicast data to the endpoint using the LE connectivity.
 *
 * @param[in] endpoint Remote Endpoint information to which the
 *                     multicast data has to be sent.
 * @param[in] data     Data which required to be sent.
 * @param[in] dataLen  Size of data to be sent.
 *
 * @note  dataLen must be > 0.
 *
 * @return The number of bytes sent on the network, or -1 on error.
 */
static int32_t CASendLEMulticastData(const CAEndpoint_t *endpoint,
                                     const void *data,
                                     uint32_t dataLen,
                                     CADataType_t dataType);

/**
 * Get LE Connectivity network information.
 *
 * @param[out] info Local connectivity information structures.
 * @param[out] size Number of local connectivity structures.
 *
 * @return ::CA_STATUS_OK or Appropriate error code.
 */
static CAResult_t CAGetLEInterfaceInformation(CAEndpoint_t **info,
                                              uint32_t *size);

/**
 * Read Synchronous API callback.
 *
 * @return  ::CA_STATUS_OK or Appropriate error code.
 */
static CAResult_t CAReadLEData();

/**
 * Stopping the adapters and close socket connections.
 *
 * LE Stops all GATT servers and GATT Clients.
 *
 * @return ::CA_STATUS_OK or Appropriate error code.
 */
static CAResult_t CAStopLE();

/**
 * Terminate the LE connectivity adapter.
 *
 * Configuration information will be deleted from further use.
 */
static void CATerminateLE();

/**
 * This function will receive the data from the GattServer and add the
 * data to the Server receiver queue.
 *
 * @param[in] remoteAddress Remote address of the device from where
 *                          data is received.
 * @param[in] data          Actual data received from the remote
 *                          device.
 * @param[in] dataLength    Length of the data received from the
 *                          remote device.
 * @param[in] sentLength    Length of the data sent from the remote
 *                          device.
 *
 * @return ::CA_STATUS_OK or Appropriate error code.
 * @retval ::CA_STATUS_OK  Successful.
 * @retval ::CA_STATUS_INVALID_PARAM  Invalid input arguments.
 * @retval ::CA_STATUS_FAILED Operation failed.
 *
 */
static CAResult_t CALEAdapterServerReceivedData(const char *remoteAddress,
                                                const uint8_t *data,
                                                uint32_t dataLength,
                                                uint32_t *sentLength);

/**
 * This function will receive the data from the GattClient and add the
 * data into the Client receiver queue.
 *
 * @param[in] remoteAddress Remote address of the device from where
 *                          data is received.
 * @param[in] data          Actual data recevied from the remote
 *                          device.
 * @param[in] dataLength    Length of the data received from the
 *                          remote device.
 * @param[in] sentLength    Length of the data sent from the remote
 *                          device.
 *
 * @return ::CA_STATUS_OK or Appropriate error code.
 * @retval ::CA_STATUS_OK  Successful.
 * @retval ::CA_STATUS_INVALID_PARAM  Invalid input arguments.
 * @retval ::CA_STATUS_FAILED Operation failed.
 */
static CAResult_t CALEAdapterClientReceivedData(const char *remoteAddress,
                                                const uint8_t *data,
                                                uint32_t dataLength,
                                                uint32_t *sentLength);

/**
 * Set the NetworkPacket received callback to CA layer from adapter
 * layer.
 *
 * @param[in] callback Callback handle sent from the upper layer.
 */
static void CASetLEReqRespAdapterCallback(CANetworkPacketReceivedCallback callback);

/**
 * Push the data from CA layer to the Sender processor queue.
 *
 * @param[in] remoteEndpoint Remote endpoint information of the
 *                           server.
 * @param[in] data           Data to be transmitted from LE.
 * @param[in] dataLen        Length of the Data being transmitted.
 *
 * @return ::CA_STATUS_OK or Appropriate error code.
 * @retval ::CA_STATUS_OK  Successful.
 * @retval ::CA_STATUS_INVALID_PARAM  Invalid input arguments.
 * @retval ::CA_STATUS_FAILED Operation failed.
 */
static CAResult_t CALEAdapterServerSendData(const CAEndpoint_t *remoteEndpoint,
                                            const uint8_t *data,
                                            uint32_t dataLen);

/**
 * Push the data from CA layer to the Sender processor queue.
 *
 * @param[in] remoteEndpoint Remote endpoint information of the
 *                           server.
 * @param[in] data           Data to be transmitted from LE.
 * @param[in] dataLen        Length of the Data being transmitted.
 *
 * @return ::CA_STATUS_OK or Appropriate error code.
 * @retval ::CA_STATUS_OK  Successful.
 * @retval ::CA_STATUS_INVALID_PARAM  Invalid input arguments.
 * @retval ::CA_STATUS_FAILED Operation failed.
 */
static CAResult_t CALEAdapterClientSendData(const CAEndpoint_t *remoteEndpoint,
                                            const uint8_t *data,
                                            uint32_t dataLen);

static CAResult_t CALEAdapterGattServerStart()
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "Before CAStartLEGattServer");

    CAResult_t result = CAStartLEGattServer();

#ifndef SINGLE_THREAD
    /*
      Don't start the server side sending queue thread until the
      server itself has actually started.
    */
    if (CA_STATUS_OK == result)
    {
        ca_mutex_lock(g_bleServerSendDataMutex);
        result = CAQueueingThreadStart(g_bleServerSendQueueHandle);
        ca_mutex_unlock(g_bleServerSendDataMutex);

        if (CA_STATUS_OK != result)
        {
            OIC_LOG_V(ERROR,
                      CALEADAPTER_TAG,
                      "Unable to start server queuing thread (%d)",
                      result);
        }
    }
#endif

    return result;
}

static CAResult_t CALEAdapterGattServerStop()
{
#ifndef SINGLE_THREAD
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "CALEAdapterGattServerStop");

    CAResult_t result = CAStopLEGattServer();
    ca_mutex_lock(g_bleServerSendDataMutex);
    if (CA_STATUS_OK == result)
    {
        result = CAQueueingThreadStop(g_bleServerSendQueueHandle);
    }
    ca_mutex_unlock(g_bleServerSendDataMutex);

    return result;
#else
    return CAStopLEGattServer();
#endif
}

static CAResult_t CALEAdapterGattClientStart()
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "Before CAStartLEGattClient");

    CAResult_t result = CAStartLEGattClient();

#ifndef SINGLE_THREAD
    /*
      Don't start the client side sending queue thread until the
      client itself has actually started.
    */
    if (CA_STATUS_OK == result)
    {
        ca_mutex_lock(g_bleClientSendDataMutex);
        result = CAQueueingThreadStart(g_bleClientSendQueueHandle);
        ca_mutex_unlock(g_bleClientSendDataMutex);

        if (CA_STATUS_OK != result)
        {
            OIC_LOG(ERROR,
                    CALEADAPTER_TAG,
                    "Unable to start client queuing thread");
        }
    }
#endif

    return result;
}

static CAResult_t CALEAdapterGattClientStop()
{
#ifndef SINGLE_THREAD
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "CALEAdapterGattClientStop");
    CAStopLEGattClient();

    ca_mutex_lock(g_bleClientSendDataMutex);
    CAResult_t result = CAQueueingThreadStop(g_bleClientSendQueueHandle);
    ca_mutex_unlock(g_bleClientSendDataMutex);

    return result;
#else
    CAStopLEGattClient();

    return CA_STATUS_OK;
#endif
}

CAResult_t CAInitializeLE(CARegisterConnectivityCallback registerCallback,
                          CANetworkPacketReceivedCallback reqRespCallback,
                          CAAdapterChangeCallback netCallback,
                          CAConnectionChangeCallback connCallback,
                          CAErrorHandleCallback errorCallback,
                          ca_thread_pool_t handle)
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    //Input validation
    VERIFY_NON_NULL(registerCallback, CALEADAPTER_TAG, "RegisterConnectivity callback is null");
    VERIFY_NON_NULL(reqRespCallback, CALEADAPTER_TAG, "PacketReceived Callback is null");
    VERIFY_NON_NULL(netCallback, CALEADAPTER_TAG, "NetworkChange Callback is null");
    VERIFY_NON_NULL(connCallback, CALEADAPTER_TAG, "ConnectionChange Callback is null");

    CAResult_t result = CA_STATUS_OK;
    result = CAInitLEAdapterMutex();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "CAInitBleAdapterMutex failed!");
        return CA_STATUS_FAILED;
    }

    result = CAInitializeLENetworkMonitor();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "CAInitializeLENetworkMonitor() failed");
        return CA_STATUS_FAILED;
    }
    CAInitializeLEAdapter();

    CASetLEClientThreadPoolHandle(handle);

    result = CAInitializeLEGattClient();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "CAInitializeLEGattClient() failed");
        return CA_STATUS_FAILED;
    }

    CASetLEReqRespClientCallback(CALEAdapterClientReceivedData);
    CASetLEServerThreadPoolHandle(handle);
    result = CAInitializeLEGattServer();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "CAInitializeLEGattServer() failed");
        return CA_STATUS_FAILED;
    }

    CASetLEAdapterThreadPoolHandle(handle);
    CASetLEReqRespServerCallback(CALEAdapterServerReceivedData);
    CASetLEReqRespAdapterCallback(reqRespCallback);

    CASetBLEClientErrorHandleCallback(CALEErrorHandler);
    CASetBLEServerErrorHandleCallback(CALEErrorHandler);
    CALERegisterNetworkNotifications(netCallback, connCallback);

    g_errorHandler = errorCallback;

    static const CAConnectivityHandler_t connHandler =
        {
            .startAdapter = CAStartLE,
            .stopAdapter = CAStopLE,
            .startListenServer = CAStartLEListeningServer,
            .stopListenServer = CAStopLEListeningServer,
            .startDiscoveryServer = CAStartLEDiscoveryServer,
            .sendData = CASendLEUnicastData,
            .sendDataToAll = CASendLEMulticastData,
            .GetnetInfo = CAGetLEInterfaceInformation,
            .readData = CAReadLEData,
            .terminate = CATerminateLE,
            .cType = CA_ADAPTER_GATT_BTLE
        };
    NonIoTivity_Init(); // ashok added
    registerCallback(connHandler);

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");

    return CA_STATUS_OK;
}

static CAResult_t CAStartLE()
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "CAStartLE");

    return CAStartLEAdapter();
}

static CAResult_t CAStopLE()
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");
#ifndef SINGLE_THREAD
    CAStopLEQueues();
#endif

    ca_mutex_lock(g_bleIsServerMutex);
    switch (g_adapterType)
    {
        case ADAPTER_SERVER:
            CALEAdapterGattServerStop();
            break;
        case ADAPTER_CLIENT:
            CALEAdapterGattClientStop();
            break;
        case ADAPTER_BOTH_CLIENT_SERVER:
            CALEAdapterGattServerStop();
            CALEAdapterGattClientStop();
            break;
        default:
            break;
    }
    ca_mutex_unlock(g_bleIsServerMutex);

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");

    return CAStopLEAdapter();
}

static void CATerminateLE()
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    CASetLEReqRespServerCallback(NULL);
    CASetLEReqRespClientCallback(NULL);
    CALERegisterNetworkNotifications(NULL, NULL);
    CASetLEReqRespAdapterCallback(NULL);
    CATerminateLENetworkMonitor();

    ca_mutex_lock(g_bleIsServerMutex);
    switch (g_adapterType)
    {
        case ADAPTER_SERVER:
            CATerminateLEGattServer();
            break;
        case ADAPTER_CLIENT:
            CATerminateLEGattClient();
            break;
        case ADAPTER_BOTH_CLIENT_SERVER:
            CATerminateLEGattServer();
            CATerminateLEGattClient();
            break;
        default:
            break;
    }
    g_adapterType = ADAPTER_EMPTY;
    ca_mutex_unlock(g_bleIsServerMutex);

#ifndef SINGLE_THREAD
    CATerminateLEQueues();
#endif
    CATerminateLEAdapterMutex();

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
}

static CAResult_t CAStartLEListeningServer()
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN - CAStartLEListeningServer");
#ifndef ROUTING_GATEWAY
    CAResult_t result = CA_STATUS_OK;
#ifndef SINGLE_THREAD
    result = CAInitLEServerQueues();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "CAInitLEServerQueues failed");
        return result;
    }
#endif

    ca_mutex_lock(g_bleIsServerMutex);
    switch (g_adapterType)
    {
        case ADAPTER_CLIENT:
            g_adapterType = ADAPTER_BOTH_CLIENT_SERVER;
            break;
        case ADAPTER_BOTH_CLIENT_SERVER:
            break;
        default:
            g_adapterType = ADAPTER_SERVER;
    }
    ca_mutex_unlock(g_bleIsServerMutex);

    result = CAGetLEAdapterState();
    if (CA_STATUS_OK != result)
    {
        if (CA_ADAPTER_NOT_ENABLED == result)
        {
            OIC_LOG(DEBUG,
                    CALEADAPTER_TAG,
                    "Listen Server will be started once BT Adapter is enabled");
            result = CA_STATUS_OK;
        }
    }
    else
    {
        result = CALEAdapterGattServerStart();
    }

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    return result;
#else
    // Routing Gateway only supports BLE client mode.
    OIC_LOG(ERROR, CALEADAPTER_TAG, "LE server not supported in Routing Gateway");
    return CA_NOT_SUPPORTED;
#endif
}

static CAResult_t CAStopLEListeningServer()
{
    OIC_LOG(ERROR, CALEADAPTER_TAG, "Listen server stop not supported.");
    return CA_NOT_SUPPORTED;
}

static CAResult_t CAStartLEDiscoveryServer()
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN - CAStartLEDiscoveryServer");
    CAResult_t result = CA_STATUS_OK;
#ifndef SINGLE_THREAD
    result = CAInitLEClientQueues();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "CAInitLEClientQueues failed");
        return result;
    }
#endif

    ca_mutex_lock(g_bleIsServerMutex);
    switch (g_adapterType)
    {
        case ADAPTER_SERVER:
            g_adapterType = ADAPTER_BOTH_CLIENT_SERVER;
            break;
        case ADAPTER_BOTH_CLIENT_SERVER:
            break;
        default:
            g_adapterType = ADAPTER_CLIENT;
    }
    ca_mutex_unlock(g_bleIsServerMutex);

    result = CAGetLEAdapterState();
    if (CA_STATUS_OK != result)
    {
        if (CA_ADAPTER_NOT_ENABLED == result)
        {
            OIC_LOG(DEBUG,
                    CALEADAPTER_TAG,
                    "Discovery Server will be started once BT Adapter is enabled");
            result = CA_STATUS_OK;
        }
    }
    else
    {
        result = CALEAdapterGattClientStart();
    }

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    return result;
}

static CAResult_t CAReadLEData()
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");
#ifdef SINGLE_THREAD
    CACheckLEData();
#endif
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

static int32_t CASendLEUnicastData(const CAEndpoint_t *endpoint,
                                   const void *data,
                                   uint32_t dataLen,
                                   CADataType_t dataType)
{
    OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "IN - CASendLEUnicastData : type(%d)", dataType);

    //Input validation
    VERIFY_NON_NULL_RET(endpoint, CALEADAPTER_TAG, "Remote endpoint is null", -1);
    VERIFY_NON_NULL_RET(data, CALEADAPTER_TAG, "Data is null", -1);

    CAResult_t result = CA_STATUS_FAILED;

    OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "g_adapterType: %d", g_adapterType);
    if (ADAPTER_EMPTY == g_adapterType)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "g_adapterType is Empty");
    }

    ca_mutex_lock(g_bleIsServerMutex);
    if (ADAPTER_SERVER == g_adapterType ||
            (ADAPTER_BOTH_CLIENT_SERVER == g_adapterType && CA_RESPONSE_DATA == dataType))
    {
        result = CALEAdapterServerSendData(endpoint, data, dataLen);
        if (CA_STATUS_OK != result)
        {
            ca_mutex_unlock(g_bleIsServerMutex);
            OIC_LOG(ERROR, CALEADAPTER_TAG, "Send unicast data for server failed");
            if (g_errorHandler)
            {
                g_errorHandler(endpoint, data, dataLen, result);
            }

            return -1;
        }
    }

    if (ADAPTER_CLIENT == g_adapterType ||
            (ADAPTER_BOTH_CLIENT_SERVER == g_adapterType && CA_REQUEST_DATA == dataType) ||
            (ADAPTER_BOTH_CLIENT_SERVER == g_adapterType && CA_RESPONSE_FOR_RES == dataType))
    {
        result = CALEAdapterClientSendData(endpoint, data, dataLen);
        if (CA_STATUS_OK != result)
        {
            ca_mutex_unlock(g_bleIsServerMutex);
            OIC_LOG(ERROR, CALEADAPTER_TAG, "Send unicast data for client failed" );

             if (g_errorHandler)
             {
                 g_errorHandler(endpoint, data, dataLen, result);
             }
            return -1;
        }
    }
    ca_mutex_unlock(g_bleIsServerMutex);

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    return dataLen;
}

static int32_t CASendLEMulticastData(const CAEndpoint_t *endpoint,
                                     const void *data,
                                     uint32_t dataLen,
                                     CADataType_t dataType)
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN - CASendLEMulticastData");

    //Input validation
    VERIFY_NON_NULL_RET(data, CALEADAPTER_TAG, "Data is null", -1);

    if (0 >= dataLen)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "Invalid Parameter");
        return -1;
    }

    CAResult_t result = CA_STATUS_FAILED;

    OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "g_adapterType: %d", g_adapterType);
    if (ADAPTER_EMPTY == g_adapterType)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "g_adapterType is Empty");
    }

    ca_mutex_lock(g_bleIsServerMutex);
    if (ADAPTER_SERVER == g_adapterType ||
            (ADAPTER_BOTH_CLIENT_SERVER == g_adapterType && CA_RESPONSE_DATA == dataType))
    {
        result = CALEAdapterServerSendData(NULL, data, dataLen);
        if (CA_STATUS_OK != result)
        {
            ca_mutex_unlock(g_bleIsServerMutex);

            OIC_LOG(ERROR, CALEADAPTER_TAG, "Send multicast data for server failed" );

            if (g_errorHandler)
            {
                g_errorHandler(endpoint, data, dataLen, result);
            }
            return -1;
        }
    }

    if (ADAPTER_CLIENT == g_adapterType ||
            (ADAPTER_BOTH_CLIENT_SERVER == g_adapterType && CA_REQUEST_DATA == dataType) ||
            (ADAPTER_BOTH_CLIENT_SERVER == g_adapterType && CA_RESPONSE_FOR_RES == dataType))
    {
        result = CALEAdapterClientSendData(NULL, data, dataLen);
        if (CA_STATUS_OK != result)
        {
            ca_mutex_unlock(g_bleIsServerMutex);

            OIC_LOG(ERROR, CALEADAPTER_TAG, "Send Multicast data for client failed" );

            if (g_errorHandler)
            {
                g_errorHandler(endpoint, data, dataLen, result);
            }
            return -1;
        }
    }
    ca_mutex_unlock(g_bleIsServerMutex);

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT - CASendLEMulticastData");
    return dataLen;
}

static CAResult_t CAGetLEInterfaceInformation(CAEndpoint_t **info, uint32_t *size)
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    VERIFY_NON_NULL(info, CALEADAPTER_TAG, "CALocalConnectivity info is null");

    char *local_address = NULL;

    CAResult_t res = CAGetLEAddress(&local_address);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "CAGetLEAddress has failed");
        return res;
    }

    if (NULL == local_address)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "local_address is NULL");
        return CA_STATUS_FAILED;
    }

    *size = 0;
    (*info) = (CAEndpoint_t *) OICCalloc(1, sizeof(CAEndpoint_t));
    if (NULL == (*info))
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "Malloc failure!");
        OICFree(local_address);
        return CA_STATUS_FAILED;
    }

    size_t local_address_len = strlen(local_address);

    if(local_address_len >= sizeof(g_localBLEAddress) ||
            local_address_len >= MAX_ADDR_STR_SIZE_CA - 1)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "local_address is too long");
        OICFree(*info);
        OICFree(local_address);
        return CA_STATUS_FAILED;
    }

    OICStrcpy((*info)->addr, sizeof((*info)->addr), local_address);
    ca_mutex_lock(g_bleLocalAddressMutex);
    OICStrcpy(g_localBLEAddress, sizeof(g_localBLEAddress), local_address);
    ca_mutex_unlock(g_bleLocalAddressMutex);

    (*info)->adapter = CA_ADAPTER_GATT_BTLE;
    *size = 1;
    OICFree(local_address);

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

static CAResult_t CALERegisterNetworkNotifications(CAAdapterChangeCallback netCallback,
                                                   CAConnectionChangeCallback connCallback)
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    ca_mutex_lock(g_bleNetworkCbMutex);
    g_networkCallback = netCallback;
    g_connectionCallback = connCallback;
    ca_mutex_unlock(g_bleNetworkCbMutex);
    CAResult_t res = CA_STATUS_OK;
    if (netCallback)
    {
        res = CASetLEAdapterStateChangedCb(CALEDeviceStateChangedCb);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG, "CASetLEAdapterStateChangedCb failed!");
        }
    }
    else
    {
        res = CAUnSetLEAdapterStateChangedCb();
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG, "CASetLEAdapterStateChangedCb failed!");
        }
    }

    if (g_connectionCallback)
    {
        res = CASetLENWConnectionStateChangedCb(CALEConnectionStateChangedCb);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, CALEADAPTER_TAG, "CASetLENWConnectionStateChangedCb failed!");
        }
    }

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    return res;
}

static void CALEConnectionStateChangedCb(CATransportAdapter_t adapter, const char* address,
                                         bool isConnected)
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN - CALEConnectionStateChangedCb");

    VERIFY_NON_NULL_VOID(address, CALEADAPTER_TAG, "address");
    (void)adapter;

#ifdef __TIZEN__
    ca_mutex_lock(g_bleIsServerMutex);
    switch (g_adapterType)
    {
        case ADAPTER_SERVER:
            CALEGattServerConnectionStateChanged(isConnected, address);
            break;
        case ADAPTER_CLIENT:
            CALEGattConnectionStateChanged(isConnected, address);
            break;
        case ADAPTER_BOTH_CLIENT_SERVER:
            CALEGattConnectionStateChanged(isConnected, address);
            CALEGattServerConnectionStateChanged(isConnected, address);
            break;
        default:
            break;
    }
    ca_mutex_unlock(g_bleIsServerMutex);
#endif

    if(!isConnected)
    {
#ifndef SINGLE_THREAD
        if(g_bleClientSenderInfo)
        {
            CALERemoveReceiveQueueData(g_bleClientSenderInfo, address);
        }

        if(g_bleServerSenderInfo)
        {
            CALERemoveReceiveQueueData(g_bleServerSenderInfo, address);
        }

        // remove data of send queue.
        if (g_bleClientSendQueueHandle)
        {
            CALERemoveSendQueueData(g_bleClientSendQueueHandle,
                                    g_bleClientSendDataMutex,
                                    address);
        }

        if (g_bleServerSendQueueHandle)
        {
            CALERemoveSendQueueData(g_bleServerSendQueueHandle,
                                    g_bleServerSendDataMutex,
                                    address);
        }
#endif
    }

    CAEndpoint_t localEndpoint = { .adapter = CA_ADAPTER_GATT_BTLE };
    OICStrcpy(localEndpoint.addr, sizeof(localEndpoint.addr), address);

    ca_mutex_lock(g_bleNetworkCbMutex);
    if (g_connectionCallback)
    {
        g_connectionCallback(&localEndpoint, isConnected);
    }
    ca_mutex_unlock(g_bleNetworkCbMutex);

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
}

static void CALEDeviceStateChangedCb(CAAdapterState_t adapter_state)
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN - CALEDeviceStateChangedCb");

    if (CA_ADAPTER_ENABLED == adapter_state)
    {
        ca_mutex_lock(g_bleIsServerMutex);
        switch (g_adapterType)
        {
            case ADAPTER_SERVER:
                CALEAdapterGattServerStart();
                break;
            case ADAPTER_CLIENT:
                CALEAdapterGattClientStart();
                break;
            case ADAPTER_BOTH_CLIENT_SERVER:
                CALEAdapterGattServerStart();
                CALEAdapterGattClientStart();
                break;
            default:
                break;
        }
        ca_mutex_unlock(g_bleIsServerMutex);
    }
    else
    {
        ca_mutex_lock(g_bleIsServerMutex);
        switch (g_adapterType)
        {
            case ADAPTER_SERVER:
                CALEAdapterGattServerStop();
                break;
            case ADAPTER_CLIENT:
                CALEAdapterGattClientStop();
                break;
            case ADAPTER_BOTH_CLIENT_SERVER:
                CALEAdapterGattServerStop();
                CALEAdapterGattClientStop();
                break;
            default:
                break;
        }
        ca_mutex_unlock(g_bleIsServerMutex);
    }

    ca_mutex_lock(g_bleNetworkCbMutex);
    if (NULL != g_networkCallback)
    {
        g_networkCallback(CA_ADAPTER_GATT_BTLE, adapter_state);
    }
    else
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "g_networkCallback is NULL");
    }
    ca_mutex_unlock(g_bleNetworkCbMutex);

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
}

static CAResult_t CALEAdapterClientSendData(const CAEndpoint_t *remoteEndpoint,
                                            const uint8_t *data,
                                            uint32_t dataLen)
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    VERIFY_NON_NULL(data, CALEADAPTER_TAG, "Param data is NULL");
#ifndef SINGLE_THREAD
    VERIFY_NON_NULL_RET(g_bleClientSendQueueHandle, CALEADAPTER_TAG,
                        "g_bleClientSendQueueHandle is  NULL",
                        CA_STATUS_FAILED);
    VERIFY_NON_NULL_RET(g_bleClientSendDataMutex, CALEADAPTER_TAG,
                        "g_bleClientSendDataMutex is NULL",
                        CA_STATUS_FAILED);

    VERIFY_NON_NULL_RET(g_bleClientSendQueueHandle, CALEADAPTER_TAG,
                        "g_bleClientSendQueueHandle",
                        CA_STATUS_FAILED);

    OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "Data Sending to LE layer [%u]", dataLen);

    CALEData_t *bleData = CACreateLEData(remoteEndpoint, data, dataLen, NULL);
    if (!bleData)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "Failed to create bledata!");
        return CA_MEMORY_ALLOC_FAILED;
    }
    // Add message to send queue
    ca_mutex_lock(g_bleClientSendDataMutex);
    CAQueueingThreadAddData(g_bleClientSendQueueHandle, bleData, sizeof(CALEData_t));
    ca_mutex_unlock(g_bleClientSendDataMutex);
#endif
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

static CAResult_t CALEAdapterServerSendData(const CAEndpoint_t *remoteEndpoint,
                                            const uint8_t *data,
                                            uint32_t dataLen)
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    VERIFY_NON_NULL(data, CALEADAPTER_TAG, "Param data is NULL");

#ifdef SINGLE_THREAD
    if (!CAIsLEConnected())
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "le not conn");
        return CA_STATUS_FAILED;
    }

    CAResult_t result = CALEServerSendDataSingleThread(data, dataLen);
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "CALEServerSendDataSingleThread failed");
        return CA_STATUS_FAILED;
    }
#else
    VERIFY_NON_NULL_RET(g_bleServerSendQueueHandle, CALEADAPTER_TAG,
                        "BleClientReceiverQueue is NULL",
                        CA_STATUS_FAILED);
    VERIFY_NON_NULL_RET(g_bleServerSendDataMutex, CALEADAPTER_TAG,
                        "BleClientSendDataMutex is NULL",
                        CA_STATUS_FAILED);

    VERIFY_NON_NULL_RET(g_bleServerSendQueueHandle, CALEADAPTER_TAG, "sendQueueHandle",
                        CA_STATUS_FAILED);

    OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "Data Sending to LE layer [%d]", dataLen);

    CALEData_t * const bleData =
        CACreateLEData(remoteEndpoint, data, dataLen, NULL);

    if (!bleData)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "Failed to create bledata!");
        return CA_MEMORY_ALLOC_FAILED;
    }

    // Add message to send queue
    ca_mutex_lock(g_bleServerSendDataMutex);
    CAQueueingThreadAddData(g_bleServerSendQueueHandle,
                            bleData,
                            sizeof(CALEData_t));
    ca_mutex_unlock(g_bleServerSendDataMutex);
#endif
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}

static CAResult_t CALEAdapterServerReceivedData(const char *remoteAddress,
                                                const uint8_t *data,
                                                uint32_t dataLength,
                                                uint32_t *sentLength)
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    //Input validation
    VERIFY_NON_NULL(data, CALEADAPTER_TAG, "Data is null");
    VERIFY_NON_NULL(sentLength, CALEADAPTER_TAG, "Sent data length holder is null");

#ifdef SINGLE_THREAD
    CALEDataReceiverHandlerSingleThread(data, dataLength);

    if (g_singleThreadReceiveData->totalDataLen == g_singleThreadReceiveData->recvDataLen)
    {
        if(g_networkPacketReceivedCallback)
        {
            // will be filled by upper layer
            const CASecureEndpoint_t endpoint =
                { .endpoint = { .adapter = CA_ADAPTER_GATT_BTLE } };

            g_networkPacketReceivedCallback(&endpoint,
                                            g_singleThreadReceiveData->defragData,
                                            g_singleThreadReceiveData->recvDataLen);
        }
        g_singleThreadReceiveData->remoteEndpoint = NULL;
        OICFree(g_singleThreadReceiveData->defragData);
        g_singleThreadReceiveData->defragData = NULL;
        OICFree(g_singleThreadReceiveData);
        g_singleThreadReceiveData = NULL;
    }
#else
    VERIFY_NON_NULL_RET(g_bleReceiverQueue,
                        CALEADAPTER_TAG,
                        "g_bleReceiverQueue",
                        CA_STATUS_FAILED);

    //Add message to data queue
    CAEndpoint_t * const remoteEndpoint =
        CACreateEndpointObject(CA_DEFAULT_FLAGS,
                               CA_ADAPTER_GATT_BTLE,
                               remoteAddress,
                               0);

    if (NULL == remoteEndpoint)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "Failed to create remote endpoint !");
        return CA_STATUS_FAILED;
    }

    // Create bleData to add to queue
    OIC_LOG_V(DEBUG,
              CALEADAPTER_TAG,
              "Data received from LE layer [%d]",
              dataLength);

    CALEData_t * const bleData =
        CACreateLEData(remoteEndpoint, data, dataLength, g_bleServerSenderInfo);

    if (!bleData)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "Failed to create bledata!");
        CAFreeEndpoint(remoteEndpoint);
        return CA_MEMORY_ALLOC_FAILED;
    }

    CAFreeEndpoint(remoteEndpoint);
    // Add message to receiver queue
    CAQueueingThreadAddData(g_bleReceiverQueue, bleData, sizeof(CALEData_t));

    *sentLength = dataLength;
#endif
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}
static CAResult_t CALEAdapterClientReceivedData(const char *remoteAddress,
                                                const uint8_t *data,
                                                uint32_t dataLength,
                                                uint32_t *sentLength)
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    //Input validation
    VERIFY_NON_NULL(data, CALEADAPTER_TAG, "Data is null");
    VERIFY_NON_NULL(sentLength, CALEADAPTER_TAG, "Sent data length holder is null");
#ifndef SINGLE_THREAD
    VERIFY_NON_NULL_RET(g_bleReceiverQueue, CALEADAPTER_TAG,
                        "g_bleReceiverQueue",
                        CA_STATUS_FAILED);

    //Add message to data queue
    CAEndpoint_t *remoteEndpoint = CACreateEndpointObject(CA_DEFAULT_FLAGS,
                                                          CA_ADAPTER_GATT_BTLE,
                                                          remoteAddress, 0);
    if (NULL == remoteEndpoint)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "Failed to create remote endpoint !");
        return CA_STATUS_FAILED;
    }

    OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "Data received from LE layer [%u]", dataLength);

    // Create bleData to add to queue
    CALEData_t *bleData = CACreateLEData(remoteEndpoint, data,
                                         dataLength, g_bleClientSenderInfo);
    if (!bleData)
    {
        OIC_LOG(ERROR, CALEADAPTER_TAG, "Failed to create bledata!");
        CAFreeEndpoint(remoteEndpoint);
        return CA_MEMORY_ALLOC_FAILED;
    }

    CAFreeEndpoint(remoteEndpoint);
    // Add message to receiver queue
    CAQueueingThreadAddData(g_bleReceiverQueue, bleData, sizeof(CALEData_t));

    *sentLength = dataLength;
#endif
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
    return CA_STATUS_OK;
}


static void CASetLEAdapterThreadPoolHandle(ca_thread_pool_t handle)
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    ca_mutex_lock(g_bleAdapterThreadPoolMutex);
    g_bleAdapterThreadPool = handle;
    ca_mutex_unlock(g_bleAdapterThreadPoolMutex);

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
}

static void CASetLEReqRespAdapterCallback(CANetworkPacketReceivedCallback callback)
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "IN");

    ca_mutex_lock(g_bleAdapterReqRespCbMutex);

    g_networkPacketReceivedCallback = callback;

    ca_mutex_unlock(g_bleAdapterReqRespCbMutex);

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "OUT");
}

static void CALEErrorHandler(const char *remoteAddress,
                             const uint8_t *data,
                             uint32_t dataLen,
                             CAResult_t result)
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "CALEErrorHandler IN");

    VERIFY_NON_NULL_VOID(data, CALEADAPTER_TAG, "Data is null");

    CAEndpoint_t *rep = CACreateEndpointObject(CA_DEFAULT_FLAGS,
                                               CA_ADAPTER_GATT_BTLE,
                                               remoteAddress,
                                               0);

    // if required, will be used to build remote endpoint
    g_errorHandler(rep, data, dataLen, result);

    CAFreeEndpoint(rep);

    OIC_LOG(DEBUG, CALEADAPTER_TAG, "CALEErrorHandler OUT");
}

#ifndef SINGLE_THREAD
static void CALERemoveSendQueueData(CAQueueingThread_t *queueHandle, ca_mutex mutex,
                                    const char* address)
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "CALERemoveSendQueueData");

    VERIFY_NON_NULL_VOID(queueHandle, CALEADAPTER_TAG, "queueHandle");
    VERIFY_NON_NULL_VOID(address, CALEADAPTER_TAG, "address");

    ca_mutex_lock(mutex);
    while (u_queue_get_size(queueHandle->dataQueue) > 0)
    {
        OIC_LOG(DEBUG, CALEADAPTER_TAG, "get data from queue");
        u_queue_message_t *message = u_queue_get_element(queueHandle->dataQueue);
        if (NULL != message)
        {
            CALEData_t *bleData = (CALEData_t *) message->msg;
            if (bleData && bleData->remoteEndpoint)
            {
                if (!strcmp(bleData->remoteEndpoint->addr, address))
                {
                    OIC_LOG(DEBUG, CALEADAPTER_TAG, "found the message of disconnected device");
                    if (NULL != queueHandle->destroy)
                    {
                        queueHandle->destroy(message->msg, message->size);
                    }
                    else
                    {
                        OICFree(message->msg);
                    }

                    OICFree(message);
                }
            }
        }
    }
    ca_mutex_unlock(mutex);
}

static void CALERemoveReceiveQueueData(u_arraylist_t *dataInfoList, const char* address)
{
    OIC_LOG(DEBUG, CALEADAPTER_TAG, "CALERemoveReceiveQueueData");

    VERIFY_NON_NULL_VOID(dataInfoList, CALEADAPTER_TAG, "dataInfoList");
    VERIFY_NON_NULL_VOID(address, CALEADAPTER_TAG, "address");

    CABLESenderInfo_t *senderInfo = NULL;
    uint32_t senderIndex = 0;

    u_arraylist_t *portList = u_arraylist_create();
    if (CA_STATUS_OK == CALEGetPortsFromSenderInfo(address, dataInfoList, portList))
    {
        uint32_t arrayLength = u_arraylist_length(portList);
        for (uint32_t i = 0; i < arrayLength; i++)
        {
            uint16_t port = (uint16_t)(uintptr_t)u_arraylist_get(portList, i);
            OIC_LOG_V(DEBUG, CALEADAPTER_TAG, "port : %X", port);

            if (CA_STATUS_OK == CALEGetSenderInfo(address, port,
                                                  dataInfoList, &senderInfo,
                                                  &senderIndex))
            {
                u_arraylist_remove(dataInfoList, senderIndex);
                OICFree(senderInfo->defragData);
                OICFree(senderInfo->remoteEndpoint);
                OICFree(senderInfo);

                OIC_LOG(DEBUG, CALEADAPTER_TAG,
                        "SenderInfo is removed for disconnection");
            }
            else
            {
                OIC_LOG(DEBUG, CALEADAPTER_TAG, "SenderInfo doesn't exist");
            }
        }
    }
    u_arraylist_destroy(portList);
}

static CAResult_t CALEGetPortsFromSenderInfo(const char *leAddress,
                                            u_arraylist_t *senderInfoList,
                                            u_arraylist_t *portList)
{
    VERIFY_NON_NULL(leAddress,
                    CALEADAPTER_TAG,
                    "NULL BLE address argument");

    const uint32_t listLength = u_arraylist_length(senderInfoList);
    const uint32_t addrLength = strlen(leAddress);

    for (uint32_t index = 0; index < listLength; index++)
    {
        CABLESenderInfo_t *info = (CABLESenderInfo_t *) u_arraylist_get(senderInfoList, index);
        if (!info || !(info->remoteEndpoint))
        {
            continue;
        }

        if (!strncmp(info->remoteEndpoint->addr, leAddress, addrLength))
        {
            u_arraylist_add(portList, (void *)(uintptr_t)info->remoteEndpoint->port);
        }
    }

    if (u_arraylist_length(portList) != 0)
    {
        return CA_STATUS_OK;
    }
    else
    {
        return CA_STATUS_FAILED;
    }
}
#endif
