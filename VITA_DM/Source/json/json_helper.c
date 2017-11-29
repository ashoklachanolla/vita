#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "json_helper.h"
#include "VITA_DM_INTERNAL.h"
#include "helper.h"

#ifdef TEST
#include "TEST_HEADER.h"
#endif


DM_Init_Info_t dminfo;


/*************************************************************************************************************************
 **************************************************************************************************************************
 * NAME                  :   DM_get_wifi_filter()
 *
 * DESCRIPTION           :   To Get WiFi Filter Params received from Smart app
 *
 * INPUTS                :
 *                           INPUT PARAMETERS:
 *
 *                                           NAME :   	TYPE :
 *                                           DESC :
 *
 *                           OUTPUT PARAMETERS:
 *
 *                                           NAME : DM_WIFI_DSM_Filter_t  	TYPE : DM_WIFI_DSM_Filter_t(Struct)
 *                                           DESC : returns WiFi Filter Params
 **************************************************************************************************************************
 **************************************************************************************************************************/

DM_WIFI_DSM_Filter_t DM_get_wifi_filter()
{
    #ifdef TEST
        DM_WIFI_DSM_Filter_t test_wifi_filter ;

        test_wifi_filter.bIncludeWifiDeviceID = 1;
        test_wifi_filter.bIncludeWifiFrequency = 1;
        test_wifi_filter.bIncludeWifiName = 1;

        test_wifi_filter.bIncludeWifiSignal = 1;

        return test_wifi_filter;
     #endif


    return dminfo.WIFI_VITA_Filter.DM_WIFI_DSM_Filter;
}

/*************************************************************************************************************************
 **************************************************************************************************************************
 * NAME                  :   DM_get_bt_filter()
 *
 * DESCRIPTION           :   To Get BT Filter Params received from Smart app
 *
 * INPUTS                :
 *                           INPUT PARAMETERS:
 *
 *                                           NAME :   	TYPE :
 *                                           DESC :
 *
 *                           OUTPUT PARAMETERS:
 *
 *                                           NAME : DM_BT_DSM_Filter_t  	TYPE : DM_BT_DSM_Filter_t(Struct)
 *                                           DESC : returns BT Filter Params
 **************************************************************************************************************************
 **************************************************************************************************************************/

DM_BT_DSM_Filter_t DM_get_bt_filter()
{

    #ifdef TEST
        DM_BT_DSM_Filter_t test_bt_filter ;

        test_bt_filter.bIncludeBtName = 1;
        test_bt_filter.bIncludeBtDeviceID = 1;
        test_bt_filter.bIncludeBtClass = 1;

        return test_bt_filter;
    #endif

    return dminfo.BT_VITA_Filter.DM_BT_DSM_Filter;
}

DM_BLE_DSM_Filter_t DM_get_ble_filter()
{

   // #ifdef TEST
        DM_BLE_DSM_Filter_t test_ble_filter ;

        test_ble_filter.bIncludeBleName = 1;
        test_ble_filter.bIncludeBleDeviceID = 1;
        test_ble_filter.bIncludeBleClass = 1;

        return test_ble_filter;
   // #endif

   // return dminfo.BLE_VITA_Filter.DM_BLE_DSM_Filter;
}

/*************************************************************************************************************************
 **************************************************************************************************************************
 * NAME                  :   getJson(json_object* rootObj, const char* key)
 *
 * DESCRIPTION           :   To Get json key's value from json object
 *
 * INPUTS                :
 *                           INPUT PARAMETERS:
 *
 *                                           NAME : rootObj  	TYPE : json_object
 *                                           DESC : json object is passed which contain key value
 *
 *                           OUTPUT PARAMETERS:
 *
 *                                           NAME : json_object  	TYPE : json_object(json)
 *                                           DESC : returns value in json_object format
 **************************************************************************************************************************
 **************************************************************************************************************************/

json_object* getJson(json_object* rootObj, const char* key)
{
    json_object* returnObj;
    if (json_object_object_get_ex(rootObj, key, &returnObj))
    {
        return returnObj;
    }
    return NULL;
}

/*************************************************************************************************************************
 **************************************************************************************************************************
 * NAME                  :   json_parse(json_object * jobj)
 *
 * DESCRIPTION           :   To parse json and get 'Cmd' keys value
 *
 * INPUTS                :
 *                           INPUT PARAMETERS:
 *
 *                                           NAME : jobj  	TYPE : json_object
 *                                           DESC : json object is passed which contain key value
 *
 *                           OUTPUT PARAMETERS:
 *
 *                                           NAME : char  	TYPE : const char
 *                                           DESC : returns 'Cmd' key's value
 **************************************************************************************************************************
 **************************************************************************************************************************/

const char *json_parse(json_object * jobj) {

    const char *deviceType = json_object_get_string(getJson(jobj,"Cmd"));
    return deviceType;
}

/*************************************************************************************************************************
 **************************************************************************************************************************
 * NAME                  :   VITA_PARSE_Data(char *str)
 *
 * DESCRIPTION           :   To parse received mqtt payload from TL to DM for 'VITA/Scan_Status' topic and triggers the function based on 'Cmd' key value
 *
 * INPUTS                :
 *                           INPUT PARAMETERS:
 *
 *                                           NAME : str  	TYPE : char
 *                                           DESC : json object is passed which contain key value
 *
 *                           OUTPUT PARAMETERS:
 *
 *                                           NAME :   	TYPE :
 *                                           DESC :
 **************************************************************************************************************************
 **************************************************************************************************************************/

void VITA_PARSE_Data(char *str)
{
    //LOG_PRINT("string : %s\n",str);
    LOG_PRINT("string : %s\n",str);

    json_object * jobj = json_tokener_parse(str);
    const char *type = json_parse(jobj);
    LOG_PRINT("VITA_PARSE_Data ==  %s\n",json_object_get_string(getJson(jobj,"Cmd")));

    if(getJson(jobj,"Cmd"))
    {
        if(strcmp(type,"VITA_DM_Init") == 0)
        {
            LOG_PRINT("========== intialization ==========\n");

            VITA_DM_ParseInitData(jobj);
        }
        else if(strcmp(type,"VITA_DM_Scan") == 0)
        {
            LOG_PRINT("============= Scan ========\n");
            VITA_DM_searchScanDevices(jobj);
        }
        else if(strcmp(type,"VITA_DM_Scan_Abort") == 0)
        {
            LOG_PRINT("=========== Abort ========\n");
            VITA_DM_ScanAbortstatus(jobj);

        }
        else if(strcmp(type,"Discover") == 0)
        {
            LOG_PRINT("=========== Discover ========\n");
            VITA_DM_Discover();

        }
        else if(strcmp(type,"iotvity_Get") == 0)
        {

            const char *handle =json_object_get_string(getJson(jobj,"Handle"));
            printf("handle on Get : %s\n",handle);
            VITA_DM_IoTvity_Get(handle);


        }
        else if(strcmp(type,"iotvity_Put") == 0)
        {

            const char *handle =json_object_get_string(getJson(jobj,"Handle"));

            const char *data =json_object_get_string(getJson(jobj,"Data"));

            json_object * handle_str = json_tokener_parse(data);

            VITA_DM_IoTvity_Put(handle,handle_str);


        }
        else if(strcmp(type,"iotvity_Observe") == 0)
        {

            const char *handle =json_object_get_string(getJson(jobj,"Handle"));

            VITA_DM_IoTvity_Observe(handle);

        }
        else
        {
            LOG_PRINT("----------- INVALID TYPE --------\n");
            VITA_DM_create_Invalid_response(json_object_new_string("VITA_DM_INVALID_CMD"));
            return;
        }
    }
    else
    {
        LOG_PRINT("========== CMD NOT RECEIVED ==========\n");
        VITA_DM_create_Invalid_response(json_object_new_string("VITA_DM_INVALID_CMD"));
        return;

    }
}

/*************************************************************************************************************************
 **************************************************************************************************************************
 * NAME                  :   VITA_DM_ParseInitData(json_object * jobj)
 *
 * DESCRIPTION           :   To parse Init json obj received from TL(MQTT) for 'VITA/Scan_Status' topic and 'Cmd' type 'VITA_DM_Init'
 *
 * INPUTS                :
 *                           INPUT PARAMETERS:
 *
 *                                           NAME : jobj  	TYPE : json_object
 *                                           DESC : json object is passed which contain key value pair's
 *
 *                           OUTPUT PARAMETERS:
 *
 *                                           NAME :   	TYPE :
 *                                           DESC :
 **************************************************************************************************************************
 **************************************************************************************************************************/

void VITA_DM_ParseInitData(json_object * jobj)
{

    json_object *dm_sl_jobj, *submodule_dsm_jobj;
    //*submodule_cm_jobj,*submodule_dem_jobj

    dminfo.BT_VITA_Filter.DM_BT_DSM_Filter.bIncludeBtName = 0;
    dminfo.BT_VITA_Filter.DM_BT_DSM_Filter.bIncludeBtDeviceID = 0;
    dminfo.BT_VITA_Filter.DM_BT_DSM_Filter.bIncludeBtClass = 0;

    dminfo.WIFI_VITA_Filter.DM_WIFI_DSM_Filter.bIncludeWifiName = 0;
    dminfo.WIFI_VITA_Filter.DM_WIFI_DSM_Filter.bIncludeWifiSignal = 0;
    dminfo.WIFI_VITA_Filter.DM_WIFI_DSM_Filter.bIncludeWifiDeviceID = 0;
    dminfo.WIFI_VITA_Filter.DM_WIFI_DSM_Filter.bIncludeWifiFrequency = 0;


    dminfo.BLE_VITA_Filter.DM_BLE_DSM_Filter.bIncludeBleName = 1;
    dminfo.BLE_VITA_Filter.DM_BLE_DSM_Filter.bIncludeBleDeviceID = 1;
    dminfo.BLE_VITA_Filter.DM_BLE_DSM_Filter.bIncludeBleClass = 1;



    //Get DeviceID from json
    if(getJson(jobj,"DeviceID"))
    {
        const char *deviceId = json_object_get_string(getJson(jobj,"DeviceID"));
        LOG_PRINT("device ID %s\n",deviceId);

        strcpy(dminfo.DeviceId,deviceId);
    }
    else
    {
        VITA_DM_create_Invalid_response(json_object_new_string("VITA_DM_Init"));
        return;

    }
    //Get DM_SL json Obj

    if(getJson(jobj,"DM_SL"))
    {
        dm_sl_jobj = getJson(jobj,"DM_SL");

        char *url = "http://192.168.4.19:8856/iotpoc/user/addUserDeviceDetails";


        strcpy(dminfo.SM_URL,"http://");
        strcat(dminfo.SM_URL,json_object_get_string(getJson(dm_sl_jobj,"RUN_IP")));
        strcat(dminfo.SM_URL,"/iotpoc/user/addUserDeviceDetails");

        LOG_PRINT("received sm_url %s\n",dminfo.SM_URL);
        LOG_PRINT("static sm_url %s\n",url);

    }
    else
    {
        VITA_DM_create_Invalid_response(json_object_new_string("VITA_DM_Init"));
        return;
    }

    //Get SubModule CM json Obj from DM_SL json Obj
    if(getJson(dm_sl_jobj,"SubModule : CM"))
    {
        // submodule_cm_jobj = getJson(dm_sl_jobj,"SubModule : CM");
    }
    else
    {
        VITA_DM_create_Invalid_response(json_object_new_string("VITA_DM_Init"));
        return;

    }

    //Get SubModule : DsM json Obj from DM_SL json Obj
    if(getJson(dm_sl_jobj,"SubModule : DsM"))
    {
        submodule_dsm_jobj = getJson(dm_sl_jobj,"SubModule : DsM");

        if(getJson(submodule_dsm_jobj,"BT_Params"))
        {
            //Get BT_Params json Obj from SubModule : DsM json Obj
            json_object *bt_params_array_jobj,*bt_params_obj;
            int bt_params_arraylen,i;

            bt_params_array_jobj = getJson(submodule_dsm_jobj,"BT_Params");

            // BT_Params is an array of objects
            bt_params_arraylen = json_object_array_length(bt_params_array_jobj);

            // LOG_PRINT("===== BT params ====\n");

            for (i = 0; i < bt_params_arraylen; i++) {

                // get the i-th object in bt_params
                bt_params_obj = json_object_array_get_idx(bt_params_array_jobj, i);

                // print out the name attribute
                //LOG_PRINT("%s\n", json_object_get_string(bt_params_obj));

                if(!strcmp(BT_Param_Name,json_object_get_string(bt_params_obj)))
                {
                    dminfo.BT_VITA_Filter.DM_BT_DSM_Filter.bIncludeBtName = 1;
                }
                else if(!strcmp(BT_Param_DeviceID,json_object_get_string(bt_params_obj)))
                {
                    dminfo.BT_VITA_Filter.DM_BT_DSM_Filter.bIncludeBtDeviceID = 1;
                }
                else if(!strcmp(BT_Param_Class,json_object_get_string(bt_params_obj)))
                {
                    dminfo.BT_VITA_Filter.DM_BT_DSM_Filter.bIncludeBtClass = 1;
                }
            }
        }

        //Get WIFI_Params json Obj from SubModule : DsM json Obj
        if(getJson(submodule_dsm_jobj,"WIFI_Params"))
        {
            //Get BT_Params json Obj from SubModule : DsM json Obj
            json_object *wifi_params_array_jobj,*wifi_params_obj;
            int wifi_params_arraylen,i;

            wifi_params_array_jobj = getJson(submodule_dsm_jobj,"WIFI_Params");

            // BT_Params is an array of objects
            wifi_params_arraylen = json_object_array_length(wifi_params_array_jobj);


            // LOG_PRINT("===== WIFI params ====\n");

            for (i = 0; i < wifi_params_arraylen; i++) {

                // get the i-th object in bt_params
                wifi_params_obj = json_object_array_get_idx(wifi_params_array_jobj, i);

                // print out the name attribute
                //LOG_PRINT("%s\n", json_object_get_string(wifi_params_obj));

                if(!strcmp(WIFI_Param_Name,json_object_get_string(wifi_params_obj)))
                {
                    dminfo.WIFI_VITA_Filter.DM_WIFI_DSM_Filter.bIncludeWifiName = 1;
                }
                else if(!strcmp(WIFI_Param_Frequency,json_object_get_string(wifi_params_obj)))
                {
                    dminfo.WIFI_VITA_Filter.DM_WIFI_DSM_Filter.bIncludeWifiFrequency = 1;
                }
                else if(!strcmp(WIFI_Param_Signal,json_object_get_string(wifi_params_obj)))
                {
                    dminfo.WIFI_VITA_Filter.DM_WIFI_DSM_Filter.bIncludeWifiSignal = 1;
                }
                else if(!strcmp(WIFI_Param_DeviceID,json_object_get_string(wifi_params_obj)))
                {
                    dminfo.WIFI_VITA_Filter.DM_WIFI_DSM_Filter.bIncludeWifiDeviceID = 1;
                }

            }
        }

    }
    else
    {
        VITA_DM_create_Invalid_response(json_object_new_string("VITA_DM_Init"));
        return;
    }

    //Get SubModule : DeM json Obj from DM_SL json Obj
    if(getJson(dm_sl_jobj,"SubModule : DeM"))
    {
        // submodule_dem_jobj = getJson(dm_sl_jobj,"SubModule : DeM");

        //Get

    }
    else
    {
        VITA_DM_create_Invalid_response(json_object_new_string("VITA_DM_Init"));
        return;
    }
    VITA_DM_Init(dminfo,dminfo.SM_URL);
    // VITA_DM_Init_ext(dminfo,dminfo.SM_URL);
    LOG_PRINT("========== intialization ==========\n");
}

/*************************************************************************************************************************
 **************************************************************************************************************************
 * NAME                  :   VITA_DM_searchScanDevices(json_object * jobj)
 *
 * DESCRIPTION           :   To parse Wifi/Bt Scan json obj received from TL(MQTT) for 'VITA/Scan_Status' topic
 *
 * INPUTS                :
 *                           INPUT PARAMETERS:
 *
 *                                           NAME : jobj  	TYPE : json_object
 *                                           DESC : json object is passed which contain key value pair's
 *
 *                           OUTPUT PARAMETERS:
 *
 *                                           NAME :   	TYPE :
 *                                           DESC :
 **************************************************************************************************************************
 **************************************************************************************************************************/
void VITA_DM_ScanAbortstatus(json_object * jobj)
{
    DM_Scan_cmd_data abort_info;

    const char *deviceType =json_object_get_string(getJson(jobj,"DeviceType"));
    const char *scanType =json_object_get_string(getJson(jobj,"ScanType"));


    strcpy(abort_info.cmd ,"VITA_DM_Scan_Abort");

    if(strcmp("AUTO",scanType) == 0)
    {
        abort_info.scanType = AUTO;
    }
    else if(strcmp("MANUAL",scanType) == 0)
    {
        abort_info.scanType = MANUAL;
    }
    else
    {
        LOG_PRINT("----------- INVALID TYPE --------\n");
        VITA_DM_create_Invalid_response(json_object_new_string("VITA_DM_Scan_Abort"));
        return;
    }

    if(strcmp (deviceType,"BT") == 0)
    {
        LOG_PRINT("----------- BT ----------\n");
        abort_info.deviceType = DEVICE_BLUETOOTH;
    }
    else if (strcmp (deviceType,"WIFI") == 0)
    {
        LOG_PRINT("--------- WIFI ---------\n");
        abort_info.deviceType = DEVICE_WIFI;
    }
    else if (strcmp (deviceType,"BLE") == 0)
    {
        LOG_PRINT("--------- BLE ---------\n");
        abort_info.deviceType = DEVICE_BLE;
    }
    else if(strcmp (deviceType,"BOTH") == 0)
    {
        LOG_PRINT("----------- BOTH -------\n");
        abort_info.deviceType = DEVICE_ALL;
    }
    else
    {
        LOG_PRINT("----------- INVALID TYPE --------\n");
        VITA_DM_create_Invalid_response(json_object_new_string("VITA_DM_Scan_Abort"));
        return;
    }
    VITA_DM_Abort(abort_info);
}

void VITA_DM_searchScanDevices(json_object * jobj)
{
    int hDMHandle;
    DM_Open_Info_t Open_info = {""};

    hDMHandle = VITA_DM_Open(Open_info);

    DM_Scan_cmd_data scan_info;

    const char *deviceType =json_object_get_string(getJson(jobj,"DeviceType"));
    const char *scanType =json_object_get_string(getJson(jobj,"ScanType"));

    //LOG_PRINT("----------- SCANNING1--------\n");

    strcpy(scan_info.cmd ,"VITA_DM_Scan");

    if(strcmp("AUTO",scanType) == 0)
    {
        scan_info.scanType = AUTO;
    }
    else if(strcmp("MANUAL",scanType) == 0)
    {
        scan_info.scanType = MANUAL;
    }
    else
    {
        LOG_PRINT("----------- INVALID TYPE --------\n");
        VITA_DM_create_Invalid_response(json_object_new_string("VITA_DM_Scan"));
        return;
    }

    if(strcmp (deviceType,"BT") == 0)
    {
        LOG_PRINT("----------- BT ----------\n");
        scan_info.deviceType = DEVICE_BLUETOOTH;
    }
    else if (strcmp (deviceType,"WIFI") == 0)
    {
        LOG_PRINT("--------- WIFI ---------\n");
        scan_info.deviceType = DEVICE_WIFI;
    }
    else if(strcmp (deviceType , "BLE") == 0)
    {
        scan_info.deviceType = DEVICE_BLE;
    }
    else if(strcmp (deviceType,"BOTH") == 0)
    {
        LOG_PRINT("----------- BOTH -------\n");
        scan_info.deviceType = DEVICE_ALL;
    }
    else
    {
        LOG_PRINT("----------- INVALID TYPE --------\n");
        VITA_DM_create_Invalid_response(json_object_new_string("VITA_DM_Scan"));
        return;
    }
    VITA_DM_Scan(1,scan_info);
}

/*************************************************************************************************************************
 **************************************************************************************************************************
 * NAME                  :   VITA_DM_create_WIFI_Response(VITA_SCAN_Type scanType ,DM_WIFI_SCAN_RESPONCE_e wifi_response)
 *
 * DESCRIPTION           :   To create wifi scan response json obj and send to SA on topic 'VITA/Store_Status'
 *
 * INPUTS                :
 *                           INPUT PARAMETERS:
 *
 *                                           NAME : scanType  	TYPE : VITA_SCAN_Type
 *                                           DESC : Indicates scan type i.e AUTO/MANUAL
 *
 *                                           NAME : wifi_response  	TYPE : DM_WIFI_SCAN_RESPONCE_e
 *                                           DESC : Wifi response enum i.e succes/fail etc
 *
 *                           OUTPUT PARAMETERS:
 *
 *                                           NAME :   	TYPE :
 *                                           DESC :
 **************************************************************************************************************************
 **************************************************************************************************************************/

void VITA_DM_create_WIFI_Response(VITA_SCAN_Type scanType ,DM_WIFI_SCAN_RESPONCE_e wifi_response)
{
    char responseString[1024];

    json_object *responsejobj;
    responsejobj = json_object_new_object();
    json_object_object_add(responsejobj, "Cmd", json_object_new_string("VITA_DM_Scan"));
    switch(wifi_response) {
    case VITA_DM_WIFI_SCAN_SUCESS:
        json_object_object_add(responsejobj, "Response", json_object_new_string("VITA_DM_SCAN_SUCESS"));
        break;
    case VITA_DM_WIFI_SCAN_FAIL:
        json_object_object_add(responsejobj, "Response", json_object_new_string("VITA_DM_SCAN_FAIL"));
        break;
    case VITA_DM_WIFI_SCAN_STORE_FAIL:
        json_object_object_add(responsejobj, "Response", json_object_new_string("VITA_DM_SCAN_STORE_FAIL"));
        break;
    case VITA_DM_WIFI_SCAN_NO_DEVICE_FOUND:
        json_object_object_add(responsejobj, "Response", json_object_new_string("VITA_DM_SCAN_NO_DEVICE_FOUND"));
        break;
    case VITA_DM_WIFI_SCAN_PROGRESS:
        json_object_object_add(responsejobj, "Response", json_object_new_string("VITA_DM_WIFI_SCAN_PROGRESS"));
        break;

    case VITA_DM_WIFI_SCAN_ABORTED:
    {
        json_object_object_add(responsejobj, "Response", json_object_new_string("VITA_DM_SCAN_ABORTED"));
        json_object_object_add(responsejobj, "Cmd", json_object_new_string("VITA_DM_ScanAbort"));
        break;
    }
    }

    if(scanType == AUTO)
    {
        json_object_object_add(responsejobj, "ScanType", json_object_new_string("AUTO"));
    }
    else if (scanType == MANUAL)
    {
        json_object_object_add(responsejobj, "ScanType", json_object_new_string("MANUAL"));
    }

    json_object_object_add(responsejobj, "DeviceType", json_object_new_string("WIFI"));

    LOG_PRINT("wifi_jsonobj_response created: %s\n",json_object_to_json_string(responsejobj));

    strcpy(responseString,json_object_to_json_string(responsejobj));

#ifndef TEST
    VITA_DM_SendResponse(responseString);
#endif
    #ifdef TEST

        if(wifi_response == VITA_DM_WIFI_SCAN_ABORTED)
        {
            Handle_WIFIScan_test_response(scanType,wifi_response,"VITA_DM_ScanAbort");
        }
        else
        {
            Handle_WIFIScan_test_response(scanType,wifi_response,"VITA_DM_Scan");
        }
    #endif
}

/*************************************************************************************************************************
 **************************************************************************************************************************
 * NAME                  :   VITA_DM_create_BT_response(VITA_SCAN_Type scanType ,DM_BT_SCAN_RESPONCE_e bt_response)
 *
 * DESCRIPTION           :   To create BT scan response json obj and send to SA on topic 'VITA/Store_Status'
 *
 * INPUTS                :
 *                           INPUT PARAMETERS:
 *
 *                                           NAME : scanType  	TYPE : VITA_SCAN_Type
 *                                           DESC : Indicates scan type i.e AUTO/MANUAL
 *
 *                                           NAME : bt_response  	TYPE : DM_BT_SCAN_RESPONCE_e
 *                                           DESC : BT response enum i.e succes/fail etc
 *
 *                           OUTPUT PARAMETERS:
 *
 *                                           NAME :   	TYPE :
 *                                           DESC :
 **************************************************************************************************************************
 **************************************************************************************************************************/

void VITA_DM_create_BT_response(VITA_SCAN_Type scanType ,DM_BT_SCAN_RESPONCE_e bt_response)
{
    char responseString[1024];

    json_object *responsejobj;
    responsejobj = json_object_new_object();
    json_object_object_add(responsejobj, "Cmd", json_object_new_string("VITA_DM_Scan"));
    switch(bt_response) {
    case VITA_DM_BT_SCAN_SUCESS:
        json_object_object_add(responsejobj, "Response", json_object_new_string("VITA_DM_SCAN_SUCESS"));
        break;
    case VITA_DM_BT_SCAN_FAIL:
        json_object_object_add(responsejobj, "Response", json_object_new_string("VITA_DM_SCAN_FAIL"));
        break;
    case VITA_DM_BT_SCAN_STORE_FAIL:
        json_object_object_add(responsejobj, "Response", json_object_new_string("VITA_DM_SCAN_STORE_FAIL"));
        break;
    case VITA_DM_BT_SCAN_NO_DEVICE_FOUND:
        json_object_object_add(responsejobj, "Response", json_object_new_string("VITA_DM_SCAN_NO_DEVICE_FOUND"));
        break;
    case VITA_DM_BT_SCAN_PROGRESS:
        json_object_object_add(responsejobj, "Response", json_object_new_string("VITA_DM_BT_SCAN_PROGRESS"));
        break;

    case VITA_DM_BT_SCAN_ABORTED:
    {
        json_object_object_add(responsejobj, "Response", json_object_new_string("VITA_DM_SCAN_ABORTED"));
        json_object_object_add(responsejobj, "Cmd", json_object_new_string("VITA_DM_ScanAbort"));
        break;
    }
    }

    if(scanType == AUTO)
    {
        json_object_object_add(responsejobj, "ScanType", json_object_new_string("AUTO"));
    }
    else if (scanType == MANUAL)
    {
        json_object_object_add(responsejobj, "ScanType", json_object_new_string("MANUAL"));
    }

    json_object_object_add(responsejobj, "DeviceType", json_object_new_string("BT"));

    LOG_PRINT("BT_jsonobj_response created: %s\n",json_object_to_json_string(responsejobj));

    strcpy(responseString,json_object_to_json_string(responsejobj));
#ifndef TEST
    VITA_DM_SendResponse(responseString);
#endif

    #ifdef TEST
        LOG_PRINT("<%s:%d>\n", __func__, __LINE__);
        //Handle_BTScan_test_response(scanType,bt_response);

        if(bt_response == VITA_DM_BT_SCAN_ABORTED)
        {
            LOG_PRINT("Hello <%s:%d>\n", __func__, __LINE__);
            Handle_BTScan_test_response(scanType,bt_response,"VITA_DM_ScanAbort");

        }
        else
        {
            Handle_BTScan_test_response(scanType,bt_response,"VITA_DM_Scan");
        }
   #endif
}

void VITA_DM_create_BLE_response(VITA_SCAN_Type scanType ,DM_BLE_SCAN_RESPONCE_e ble_response)
{
    char responseString[1024];

    json_object *responsejobj;
    responsejobj = json_object_new_object();
    json_object_object_add(responsejobj, "Cmd", json_object_new_string("VITA_DM_Scan"));
    switch(ble_response) {
    case VITA_DM_BT_SCAN_SUCESS:
        json_object_object_add(responsejobj, "Response", json_object_new_string("VITA_DM_SCAN_SUCESS"));
        break;
    case VITA_DM_BT_SCAN_FAIL:
        json_object_object_add(responsejobj, "Response", json_object_new_string("VITA_DM_SCAN_FAIL"));
        break;
    case VITA_DM_BT_SCAN_STORE_FAIL:
        json_object_object_add(responsejobj, "Response", json_object_new_string("VITA_DM_SCAN_STORE_FAIL"));
        break;
    case VITA_DM_BT_SCAN_NO_DEVICE_FOUND:
        json_object_object_add(responsejobj, "Response", json_object_new_string("VITA_DM_SCAN_NO_DEVICE_FOUND"));
        break;
    case VITA_DM_BT_SCAN_PROGRESS:
        json_object_object_add(responsejobj, "Response", json_object_new_string("VITA_DM_BLE_SCAN_PROGRESS"));
        break;

    case VITA_DM_BT_SCAN_ABORTED:
    {
        json_object_object_add(responsejobj, "Response", json_object_new_string("VITA_DM_SCAN_ABORTED"));
        json_object_object_add(responsejobj, "Cmd", json_object_new_string("VITA_DM_ScanAbort"));
        break;
    }
    }

    if(scanType == AUTO)
    {
        json_object_object_add(responsejobj, "ScanType", json_object_new_string("AUTO"));
    }
    else if (scanType == MANUAL)
    {
        json_object_object_add(responsejobj, "ScanType", json_object_new_string("MANUAL"));
    }

    json_object_object_add(responsejobj, "DeviceType", json_object_new_string("BLE"));

    //LOG_PRINT("BLE_jsonobj_response created: %s\n",json_object_to_json_string(responsejobj));

    strcpy(responseString,json_object_to_json_string(responsejobj));
#ifndef TEST
    VITA_DM_SendResponse(responseString);
#endif

    #ifdef TEST
        LOG_PRINT("<%s:%d>\n", __func__, __LINE__);
        //Handle_BTScan_test_response(scanType,bt_response);

        if(ble_response == VITA_DM_BT_SCAN_ABORTED)
        {
            LOG_PRINT("Hello <%s:%d>\n", __func__, __LINE__);
            Handle_BTScan_test_response(scanType,ble_response,"VITA_DM_ScanAbort");

        }
        else
        {
            Handle_BTScan_test_response(scanType,ble_response,"VITA_DM_Scan");
        }
   #endif
}

/*************************************************************************************************************************
 **************************************************************************************************************************
 * NAME                  :   VITA_DM_create_Init_response(DM_INIT_RESPONCE_e init_response)
 *
 * DESCRIPTION           :   To create Init response json obj and send to SA on topic 'VITA/Store_Status'
 *
 * INPUTS                :
 *                           INPUT PARAMETERS:
 *
 *
 *                                           NAME : init_response  	TYPE : DM_INIT_RESPONCE_e
 *                                           DESC : Init response enum i.e succes/fail etc
 *
 *                           OUTPUT PARAMETERS:
 *
 *                                           NAME :   	TYPE :
 *                                           DESC :
 **************************************************************************************************************************
 **************************************************************************************************************************/

void VITA_DM_create_Init_response(DM_INIT_RESPONCE_e init_response)
{
    json_object *responsejobj;
    char responseString[1024];

    responsejobj = json_object_new_object();


    LOG_PRINT("VITA_DM_create_Init_response - Init response %d\n",init_response);

    if(init_response == VITA_DM_INIT_SUCESS)
    {
        json_object_object_add(responsejobj, "Response", json_object_new_string("VITA_DM_INIT_SUCESS"));

    }
    else if (init_response == VITA_DM_INIT_FAIL)
    {
        json_object_object_add(responsejobj, "Response", json_object_new_string("VITA_DM_INIT_FAIL"));
    }
    json_object_object_add(responsejobj, "Cmd", json_object_new_string("VITA_DM_Init"));

    LOG_PRINT("wifi_jsonobj_response created: %s\n",json_object_to_json_string(responsejobj));

    strcpy(responseString,json_object_to_json_string(responsejobj));
#ifndef TEST
    VITA_DM_SendResponse(responseString);
#endif

    #ifdef TEST
        Handle_init_test_response(init_response);
    #endif
}

/*************************************************************************************************************************
 **************************************************************************************************************************
 * NAME                  :   VITA_DM_create_Invalid_response(char response[])
 *
 * DESCRIPTION           :   To create Invalid response json obj and send to SA on topic 'VITA/Store_Status'
 *
 * INPUTS                :
 *                           INPUT PARAMETERS:
 *
 *
 *                                           NAME : response  	TYPE : char
 *                                           DESC :
 *
 *                           OUTPUT PARAMETERS:
 *
 *                                           NAME :   	TYPE :
 *                                           DESC :
 **************************************************************************************************************************
 **************************************************************************************************************************/


void VITA_DM_create_Invalid_response(json_object *response)
{
    json_object *responsejobj;
    char responseString[1024];

    responsejobj = json_object_new_object();

    json_object_object_add(responsejobj, "Response", json_object_new_string("VITA_DM_INVALID_ARGUMENTS"));
    json_object_object_add(responsejobj, "Cmd", response);

    strcpy(responseString,json_object_to_json_string(responsejobj));
#ifndef TEST
    VITA_DM_SendResponse(responseString);
#endif
}
