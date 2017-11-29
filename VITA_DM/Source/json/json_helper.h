#ifndef JSON_HELPER_H
#define JSON_HELPER_H

#include <json-c/json.h>

#include "VITA_DM.h"

#ifdef __cplusplus
extern "C" {
#endif

const char *json_parse(json_object * jobj);
void VITA_PARSE_Data(char *str);
void VITA_DM_ParseInitData(json_object * jobj);
void VITA_DM_ScanAbortstatus(json_object * jobj);
void VITA_DM_searchScanDevices(json_object * jobj);
void VITA_DM_create_WIFI_Response(VITA_SCAN_Type scanType ,DM_WIFI_SCAN_RESPONCE_e wifi_response);
void VITA_DM_create_BT_response(VITA_SCAN_Type scanType ,DM_BT_SCAN_RESPONCE_e bt_response);
void VITA_DM_create_BLE_response(VITA_SCAN_Type scanType ,DM_BLE_SCAN_RESPONCE_e ble_response);
void VITA_DM_create_Init_response(DM_INIT_RESPONCE_e init_response);
void VITA_DM_create_Invalid_response(json_object *response);

#ifdef __cplusplus
}
#endif

#endif
