#include <stdio.h>
#include <stdlib.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include "BLEsearch.h"
#include <stdlib.h>
#include <errno.h>
#include <curses.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <pthread.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <stdio.h>
#include <errno.h>

#define HCI_STATE_NONE       0
#define HCI_STATE_OPEN       2
#define HCI_STATE_SCANNING   3
#define HCI_STATE_FILTERING  4
#define HCI_MAX_EVENT_SIZE   260
#define BLE_SCAN_TO_READ_DELAY 3
#define EIR_FLAGS                   0X01
#define EIR_NAME_SHORT              0x08
#define EIR_NAME_COMPLETE           0x09
#define EIR_MANUFACTURE_SPECIFIC    0xFF
#define MIN(x,y)    ((x)<(y)?(x):(y))

unsigned char buf1[HCI_MAX_EVENT_SIZE] = { 0 };

//int ble_rsp_devices = 0;

int DM_BLE_Scan(DM_BLE_ScanResult_Info_t *resultData)
{
    
unsigned int handler;
    unsigned char buf[HCI_MAX_EVENT_SIZE] = { 0 };
    int len;
    int8_t rssi;
    int error;
    char name[100];
    char addr[18];
    extern int errno;
    int ble_count = 0;

    struct hci_state open_default_hci_device ();
    struct hci_state current_hci_state = open_default_hci_device ();
    handler = current_hci_state.device_handle;
    start_hci_scan (current_hci_state);
    //int k = 0;
    while (get_advt_data (handler, buf, HCI_MAX_EVENT_SIZE) != -1)
    {
        // int i;
//        len = get_advt_data (handler, buf, HCI_MAX_EVENT_SIZE);
        sleep(1);
//        if(len == -1)
//        {
//            k = 1;
//        }
//        else
        if(len > 0)
        {
            error = get_device_name (buf, len, name, addr,&rssi);
            if (error == 0)
            {
                //insert_device_name (name, addr,rssi,ble_count);
                //memset(name, 0, sizeof(name));

                memcpy(resultData[ble_count].ble_name, name, strlen(name));
                memcpy(resultData[ble_count].ble_addr, addr, strlen(addr));
                resultData[ble_count].ble_rssi = rssi;

                ble_count++;
            }
        }
    }
    
    if (ble_count == 0)
    {
       // printf("No Ble Devices found\n");
//        return 0;
    }
    else
    {
        //display();
    }
    
    if (hci_le_set_scan_enable (current_hci_state.device_handle, 0x00, 1, 1000)
        < 0)
    {
        current_hci_state.has_error = TRUE;
        printf("Failed to disable scan");
        
        return;
    }
    
    if( hci_close_dev(current_hci_state.device_handle)<0)
    {
        current_hci_state.has_error = TRUE;
        printf("Failed to close");
        return;
    }
    
    resultData[0].num_elements = ble_count;

    return ble_count;



//    //while (1)
//    {
//        int i;

//        len = get_advt_data (handler, buf, HCI_MAX_EVENT_SIZE);
//        // printf("===============\n BLE RAW data Start================\n");
//        //for(i=0;i<len;i++)
//        // printf("%0.2x ", buf[i]);
//        // printf("================\nBLE Data END================\n");
//        error = get_device_name (buf, len, name, addr);
//        //  printf("Device name %s, btaddr %s\n",name,addr);


//        for (i = 0; i < ble_rsp_devices; i++) {
//            memset(name, 0, sizeof(name));

//            memcpy(resultData[i].ble_name, name, strlen(name));
//            memcpy(resultData[i].ble_addr, addr, strlen(addr));
//            resultData[i].ble_class = 1;

//            //!Debug the search results
//            printf("[RAW: %d]%s  %s %d\n", i, name, addr, (*(ii+i)->dev_class+0));
//            //printf("[RESULTDATA]%s  %s %d\n", resultData[i].bt_name, resultData[i].bt_addr, resultData[i].bt_class);
//        }

//        resultData[0].num_elements = ble_rsp_devices;

//        if (error == 0)
//        {
//            insert_device_name (name, addr);
//        }

//    }
//    return ble_rsp_devices;
}


struct hci_state
open_default_hci_device ()
{
    struct hci_state current_hci_state = { 0 };
    
    current_hci_state.device_id = hci_get_route (NULL);
    
    if ((current_hci_state.device_handle =
         hci_open_dev (current_hci_state.device_id)) < 0)
    {
        current_hci_state.has_error = TRUE;
        snprintf (current_hci_state.error_message,
                  sizeof (current_hci_state.error_message),
                  "Could not open device: %s", strerror (errno));
        return current_hci_state;
    }
    
    // Set fd non-blocking
    int on = 1;
    if (ioctl (current_hci_state.device_handle, FIONBIO, (char *) &on) < 0)
    {
        current_hci_state.has_error = TRUE;
        snprintf (current_hci_state.error_message,
                  sizeof (current_hci_state.error_message),
                  "Could set device to non-blocking: %s", strerror (errno));
        return current_hci_state;
    }
    if (ioctl(current_hci_state.device_handle, HCIDEVDOWN, 0) < 0) {
        
        perror("error in device down");
        
    }
    if (ioctl(current_hci_state.device_handle, HCIDEVUP, 0) < 0) {
        
        perror("error in device up");
    }
    
    current_hci_state.state = HCI_STATE_OPEN;
    
    return current_hci_state;
}

void
start_hci_scan (struct hci_state current_hci_state)
{
    if (hci_le_set_scan_parameters
        (current_hci_state.device_handle, 0x01, htobs (0x0640), htobs (0x0640),
         0x00, 0x00, 1000) < 0)
    {
        current_hci_state.has_error = TRUE;
        snprintf (current_hci_state.error_message,
                  sizeof (current_hci_state.error_message),
                  "Failed to set scan parameters: %s", strerror (errno));
        printf("error in return\n");
        return;
    }
    
    if (hci_le_set_scan_enable (current_hci_state.device_handle, 0x01, 1, 1000)
        < 0)
    {
        current_hci_state.has_error = TRUE;
        snprintf (current_hci_state.error_message,
                  sizeof (current_hci_state.error_message),
                  "Failed to enable scan: %s", strerror (errno));
        return;
    }
    current_hci_state.state = HCI_STATE_SCANNING;
    
    // Save the current HCI filter
    socklen_t olen = sizeof (current_hci_state.original_filter);
    if (getsockopt
        (current_hci_state.device_handle, SOL_HCI, HCI_FILTER,
         &current_hci_state.original_filter, &olen) < 0)
    {
        current_hci_state.has_error = TRUE;
        snprintf (current_hci_state.error_message,
                  sizeof (current_hci_state.error_message),
                  "Could not get socket options: %s", strerror (errno));
        return;
    }
    
    // Create and set the new filter
    struct hci_filter new_filter;
    hci_filter_clear (&new_filter);
    hci_filter_set_ptype (HCI_EVENT_PKT, &new_filter);
    hci_filter_set_event (EVT_LE_META_EVENT, &new_filter);
    
    if (setsockopt
        (current_hci_state.device_handle, SOL_HCI, HCI_FILTER, &new_filter,
         sizeof (new_filter)) < 0)
    {
        current_hci_state.has_error = TRUE;
        snprintf (current_hci_state.error_message,
                  sizeof (current_hci_state.error_message),
                  "Could not set socket options: %s", strerror (errno));
        return;
    }
    
    current_hci_state.state = HCI_STATE_FILTERING;
    sleep(BLE_SCAN_TO_READ_DELAY);
}

int
get_advt_data (unsigned int handler, unsigned char *buf, int buf_len)
{
    int len = 0;
    memset(buf,0,buf_len);
    while ((len = read (handler, buf, buf_len)) < 0)
    {
        return -1;
    }
    return len;
}

int
get_device_name (unsigned char *buf, unsigned int len, unsigned char *name,
                 unsigned char *addr,int8_t *rssi)
{
    hci_event_hdr *hdr = (void *) (buf + 1);
    evt_le_meta_event *meta = (void *) buf + (1 + HCI_EVENT_HDR_SIZE);
    le_advertising_info *info = (le_advertising_info *) (meta->data + 1);
    int error = 1;
    int current_index = 0;
    int data_error = 0;
    uint8_t *data;
    data = malloc (100);
    len -= (1 + HCI_EVENT_HDR_SIZE);
    // Determine Range(RSSI)
    int8_t rssi1;
    rssi1 = info->data[info->length];
    /*if ((uint8_t) rssi1 == 0x99 || rssi1 == 127)
    {
        printf("RSSI: invalid (0x%2.2x)\n", (uint8_t) rssi1);
    }
    else
    {
        printf("RSSI: %d dBm (0x%2.2x)\n", rssi1, (uint8_t) rssi1);
    }*/
    
    switch (hdr->evt)
    {
        case EVT_CMD_STATUS:
            printf("EVT_CMD_STATUS");
            break;
            
        case EVT_CMD_COMPLETE:
            printf("EVT_CMD_COMPLETE");
            break;
            
        case EVT_REMOTE_NAME_REQ_COMPLETE:
            printf("EVT_REMOTE_NAME_REQ_COMPLETE");
            break;
            
        case EVT_LE_META_EVENT:
            
            while (!data_error && current_index < info->length)
            {
                size_t data_len = info->data[current_index];
                if (data_len + 1 > info->length)
                {
                    data_error = 1;
                }
                else
                {
                    data = info->data + current_index + 1;
                    if (data[0] == EIR_NAME_SHORT || data[0] == EIR_NAME_COMPLETE)
                    {
                        size_t name_len = data_len - 1;
                        memset (name, 0, name_len + 1);
                        memcpy (name, &data[1], name_len);
                        
                        ba2str (&info->bdaddr, addr);
                        // printf(" RSSI %d\n", (char)info->data[info->length]);
                        error = 0;
                    }
                    else
                    {
                        ba2str(&(info->bdaddr), addr);
                        memset (name, 0, 7 + 1);
                        memcpy (name, "Unknown", 7);
                        // printf(" RSSI %d\n", (char)info->data[info->length]);
                        error = 0;
                    }
                    *rssi = rssi1;
                    current_index += data_len + 1;
                }
                return error;
            }			//moving to next stucture within a packet
            break;
        default:
            break;
    }
}

//int insert_device_name (char *name, char *addr,int8_t rssi,int count)
//{
//    strcpy (ble_devices[count].name, name);
//    strcpy (ble_devices[count].addr, addr);
//    ble_devices[count].rssi = rssi;
//    ble_devices[count].count = count;
//}

//void *display ()
//{
//    {
//        int i;
//        for(i = 0; i<ble_count; i++ )
//        {
//           // printf("%d Name %s addr %s, rssi %d\n",i,ble_devices[i].name,ble_devices[i].addr,ble_devices[i].rssi);
//        }
        
//    }
//}

//BLE CODE END
