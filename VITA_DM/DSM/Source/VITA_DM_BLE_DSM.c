#include "VITA_DM_DSM.h"
#include "helper.h"

DM_BLE_ScanResult_Info_t BLEresult[MAX_SCAN_ELEMENTS];



json_object *ble_prepare_url(DM_BLE_ScanResult_Info_t *BLESearchData)
{
    int i;

    json_object *json;
    json_object *jarray;

    json_object *json_sub_ble;

    json = json_object_new_object();


    char *deviceid;

    int result = DM_Get_Device_ID(&deviceid);
    if(result <= 0)
    {
       // LOG_PRINT("<%s:%d>Empty device ID\n", __func__, __LINE__);
        deviceid = json_object_new_string(VITA_helper_macaddr(DEVICE_WIFI));
    }
    else
    {
       // LOG_PRINT("<%s:%d>received device ID %s\n", __func__, __LINE__,deviceid);

    }

    json_object_object_add(json, "DeviceID", json_object_new_string(deviceid));
    json_object_object_add(json, "scan_timestamp", json_object_new_string(VITA_DM_getTimeStamp(DEVICE_BLE)));
    json_object_object_add(json, "scan_type", json_object_new_string("BLE"));

    //json_object_object_add(json, "DeviceID", json_object_new_string(VITA_helper_macaddr(DEVICE_WIFI)));

    //Creating a json array
    jarray = json_object_new_array();
    //!Preparing data to post from the search results

    DM_BLE_DSM_Filter_t  dm_ble_filter_parameters;
    dm_ble_filter_parameters = DM_get_ble_filter();
//    DM_BT_DSM_Filter_t  dm_bt_filter_parameters = DM_get_bt_filter();



    if(BLESearchData[0].num_elements)
    {
        printf("=================================================\n");
        for(i = 0; i < BLESearchData[0].num_elements; i++) {
            //!Debug the search results
            //LOG_PRINT("%s:%d %d. %s	%s \n", __func__, __LINE__, i, (BTSearchData[i]).bt_addr, (BTSearchData[i]).bt_name);
            json_sub_ble = json_object_new_object();
            if(dm_ble_filter_parameters.bIncludeBleName == 1)
            {
                json_object_object_add(json_sub_ble, "Name", json_object_new_string(BLESearchData[i].ble_name));
            }
            if(dm_ble_filter_parameters.bIncludeBleDeviceID == 1)
            {
                json_object_object_add(json_sub_ble, "pid", json_object_new_string(BLESearchData[i].ble_addr));
            }
            if(dm_ble_filter_parameters.bIncludeBleClass == 1)
            {
                json_object_object_add(json_sub_ble, "Rssi", json_object_new_int(BLESearchData[i].ble_rssi));
            }
            json_object_object_add(json_sub_ble, "type", json_object_new_string("BLE"));
            json_object_array_add(jarray,json_sub_ble);

            printf("%d.Name : %s, Device ID : %s, RSSI : %d \n",i+1,BLESearchData[i].ble_name,BLESearchData[i].ble_addr,BLESearchData[i].ble_rssi);
        }

        printf("=================================================\n\n");

    }


    json_object_object_add(json,"userNetworkDetails", jarray);

    //!TODO: free the memory allocated for macaddr capture

    //!JSON final payload
    //LOG_PRINT("The json object created: %s\n",json_object_to_json_string(json));
    return json;
}


int VITA_DM_Das_BLE_Send(DM_BLE_ScanResult_Info_t *BLESearchData)
{
    int result;
    json_object *jsonString;
    jsonString = ble_prepare_url(BLESearchData);
    char *url;
    int url_result = DM_GetSM_URL(&url);
    if(url_result <= 0)
    {
        url = "http://192.168.4.19:8856/iotpoc/user/addUserDeviceDetails";
    }

//    result = VITA_DM_sendData(jsonString,url);

    //result = VITA_DM_sendData(jsonString);
    result = 0; //REmove this line when BLE implemented at server side
    return result;
}
