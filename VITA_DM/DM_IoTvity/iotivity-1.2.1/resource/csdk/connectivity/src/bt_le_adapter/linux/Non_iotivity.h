#include "../../../api/cacommon.h"

#define BT_ATT_OP_WRITE_REQ   0x12

#define ATT_CID 4
#define BT_ATT_OP_MTU_REQ			0x02
#define BT_ATT_OP_FIND_INFO_REQ			0x04
#define BT_ATT_OP_READ_BY_GRP_TYPE_REQ		0x10
#define BT_ATT_OP_FIND_INFO_RSP			0x05
#define BT_ATT_OP_READ_REQ			0x0a
#define BT_ATT_OP_WRITE_REQ			0x12
#define BT_ATT_OP_WRITE_RSP			0x13


#define MAX_SIZE 200
#define BT_ATT_OP_ERROR_RSP			0x01

#define MAX_BLE_DEVICES 100
#define MAX_URI_NAME 50
#define MAX_DEVICE_NAME 50
#define MAX_JSON_KEY 50
#define MAX_MAC_ID 50
#define CHAR_TABLE_LENGTH 200

#define PRLOG(...) \
    printf(__VA_ARGS__); print_prompt();

#define MAX_SIZE 200
#define BT_ATT_OP_ERROR_RSP			0x01

#define MAX_BLE_DEVICES 100
#define MAX_JSON_VALUE 512

typedef struct
{
  uint16_t uuid;
  const char *str;
} uuid_table_t;

//typedef struct key_value_pair
//{
//  unsigned char key[28];
//  unsigned char value[512];
//  uint16_t handle_res;
//  struct key_value_pair *pNext;
//} key_value_pair_t;

typedef struct information
{
  unsigned char type[16];
  unsigned char value[512];
  uint16_t handle;
} info;

typedef unsigned short int uint16_t;
typedef unsigned char 	uint8_t;

typedef enum{
    DISCOVERED
}Device_State;

typedef struct
{
  char type[16];
  char value[512];
  uint16_t handle;
} Characteristic_Table_t;

//typedef struct{
//    char key[MAX_JSON_KEY];
//    char value[MAX_JSON_VALUE];
//}json_key_value;

//typedef struct json_list
//{
//    json_key_value obj;
//    struct json_list *next;
//}JSONObj;


typedef struct
{
    unsigned char ucURI[MAX_URI_NAME];
    unsigned char ucDeviceName[MAX_DEVICE_NAME];
    unsigned char ucMacId[MAX_MAC_ID];
    key_value_pair_t1 *pJSON;  //key_value_pair_t
    Characteristic_Table_t GattTable[CHAR_TABLE_LENGTH];
    unsigned char GattTableIndex;
    Device_State eState;
}BLE_DeviceInfo_t;

typedef struct
{
    unsigned char ucNumOfDevices;
    BLE_DeviceInfo_t BLE_DeviceInfo[MAX_BLE_DEVICES];
}BLE_Device_InfoTable_t;




