#ifndef CONFIGURE_BT_H
#define CONFIGURE_BT_H

#include "VITA_DM_INTERNAL.h"

#ifdef __cplusplus
extern "C" {
#endif
int DM_BLE_Scan(DM_BLE_ScanResult_Info_t *resultData);
DM_BLE_ScanResult_Info_t BLEresult[MAX_SCAN_ELEMENTS];

//BLE HEADERS START
struct hci_state
{
  int device_id;
  int device_handle;
  struct hci_filter original_filter;
  int state;
  int has_error;
  char error_message[1024];
};

struct device_info
{
  char name[100];
  char addr[18];
  int count;
};
//struct device_info device[LIST_SIZE];

void init ();

struct hci_state open_default_hci_device ();
void start_hci_scan (struct hci_state current_hci_state);
void *display ();
//int insert_device_name (char *name, char *addr);
int get_advt_data (unsigned int handler, unsigned char *buf, int buf_len);
int get_device_name (unsigned char *buf, unsigned int len, unsigned char *name,unsigned char *addr,int8_t *rssi);

//BLE HEADERS END

#ifdef __cplusplus
}
#endif

#endif //CONFIGURE_BT_H
