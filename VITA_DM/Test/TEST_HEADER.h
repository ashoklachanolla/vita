#ifndef TEST_HEADER_H
#define TEST_HEADER_H

#include "VITA_DM.h"

void Handle_init_test_response(DM_INIT_RESPONCE_e init_response);
void Handle_BTScan_test_response(VITA_SCAN_Type scanType ,DM_BT_SCAN_RESPONCE_e bt_response,char *scan_cmd);
void Handle_WIFIScan_test_response(VITA_SCAN_Type scanType ,DM_WIFI_SCAN_RESPONCE_e bt_response,char *scan_cmd);

#endif
