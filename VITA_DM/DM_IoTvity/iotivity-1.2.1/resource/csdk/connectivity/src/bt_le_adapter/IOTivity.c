#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <limits.h>
#include <errno.h>
#include <pthread.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/l2cap.h>

CAResult_t CALEAdapterClientReceivedDataEX(const char *remoteAddress,
                                                const uint8_t *data,
                                                uint32_t dataLength,
                                                uint32_t *sentLength);
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

    return;
}
//*************************************
bool check_for_ble_status_flag()
{
    if(connect_status == 1)
            return true;
    else
        return false;

}

//****************VITA_DM_BLE_IMPLEMENTATION***************


int update_status(int value)
{

    connect_status = value;

}
static int l2cap_le_att_connect(bdaddr_t *src, bdaddr_t *dst, uint8_t dst_type,
                                    int sec)
{
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
                            sizeof(btsec)) != 0)
    {
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
                            printf("Packet Up....\n");
                            count=0;
                    }//wpdu
                if(rpdu[0] == 0x13)
                {
                    printf("Response okn");
                }
            }//len
            else
            {
                sleep(1);
            }
    }//while
    return;
}
void init (char* address)
{
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

   VITA_WRITE_REQUEST(fd,wpdu,5);
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
CAResult_t CALEAdapterClientReceivedDataEX(const char *remoteAddress,
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
//****************VITA_DM_BLE_IMPLEMENTATION END*************





