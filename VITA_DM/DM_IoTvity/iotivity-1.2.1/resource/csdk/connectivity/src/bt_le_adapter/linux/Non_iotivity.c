#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <limits.h>
#include <errno.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/l2cap.h>
//#include "lib/uuid.h"
#include "Non_iotivity.h"
//#include "../../../api/cacommon.h"
 key_value_pair_t1 *testGetdata;


uuid_table_t uuid_table[] = {
  {0x0001, "SDP"},
  {0x0003, "RFCOMM"},
  {0x0005, "TCS-BIN"},
  {0x0007, "ATT"},
  {0x0008, "OBEX"},
  {0x000f, "BNEP"},
  {0x0010, "UPNP"},
  {0x0011, "HIDP"},
  {0x0012, "Hardcopy Control Channel"},
  {0x0014, "Hardcopy Data Channel"},
  {0x0016, "Hardcopy Notification"},
  {0x0017, "AVCTP"},
  {0x0019, "AVDTP"},
  {0x001b, "CMTP"},
  {0x001e, "MCAP Control Channel"},
  {0x001f, "MCAP Data Channel"},
  {0x0100, "L2CAP"},
  /* 0x0101 to 0x0fff undefined */
  {0x1000, "Service Discovery Server Service Class"},
  {0x1001, "Browse Group Descriptor Service Class"},
  {0x1002, "Public Browse Root"},
  /* 0x1003 to 0x1100 undefined */
  {0x1101, "Serial Port"},
  {0x1102, "LAN Access Using PPP"},
  {0x1103, "Dialup Networking"},
  {0x1104, "IrMC Sync"},
  {0x1105, "OBEX Object Push"},
  {0x1106, "OBEX File Transfer"},
  {0x1107, "IrMC Sync Command"},
  {0x1108, "Headset"},
  {0x1109, "Cordless Telephony"},
  {0x110a, "Audio Source"},
  {0x110b, "Audio Sink"},
  {0x110c, "A/V Remote Control Target"},
  {0x110d, "Advanced Audio Distribution"},
  {0x110e, "A/V Remote Control"},
  {0x110f, "A/V Remote Control Controller"},
  {0x1110, "Intercom"},
  {0x1111, "Fax"},
  {0x1112, "Headset AG"},
  {0x1113, "WAP"},
  {0x1114, "WAP Client"},
  {0x1115, "PANU"},
  {0x1116, "NAP"},
  {0x1117, "GN"},
  {0x1118, "Direct Printing"},
  {0x1119, "Reference Printing"},
  {0x111a, "Basic Imaging Profile"},
  {0x111b, "Imaging Responder"},
  {0x111c, "Imaging Automatic Archive"},
  {0x111d, "Imaging Referenced Objects"},
  {0x111e, "Handsfree"},
  {0x111f, "Handsfree Audio Gateway"},
  {0x1120, "Direct Printing Refrence Objects Service"},
  {0x1121, "Reflected UI"},
  {0x1122, "Basic Printing"},
  {0x1123, "Printing Status"},
  {0x1124, "Human Interface Device Service"},
  {0x1125, "Hardcopy Cable Replacement"},
  {0x1126, "HCR Print"},
  {0x1127, "HCR Scan"},
  {0x1128, "Common ISDN Access"},
  /* 0x1129 and 0x112a undefined */
  {0x112d, "SIM Access"},
  {0x112e, "Phonebook Access Client"},
  {0x112f, "Phonebook Access Server"},
  {0x1130, "Phonebook Access"},
  {0x1131, "Headset HS"},
  {0x1132, "Message Access Server"},
  {0x1133, "Message Notification Server"},
  {0x1134, "Message Access Profile"},
  {0x1135, "GNSS"},
  {0x1136, "GNSS Server"},
  {0x1137, "3D Display"},
  {0x1138, "3D Glasses"},
  {0x1139, "3D Synchronization"},
  {0x113a, "MPS Profile"},
  {0x113b, "MPS Service"},
  /* 0x113c to 0x11ff undefined */
  {0x1200, "PnP Information"},
  {0x1201, "Generic Networking"},
  {0x1202, "Generic File Transfer"},
  {0x1203, "Generic Audio"},
  {0x1204, "Generic Telephony"},
  {0x1205, "UPNP Service"},
  {0x1206, "UPNP IP Service"},
  {0x1300, "UPNP IP PAN"},
  {0x1301, "UPNP IP LAP"},
  {0x1302, "UPNP IP L2CAP"},
  {0x1303, "Video Source"},
  {0x1304, "Video Sink"},
  {0x1305, "Video Distribution"},
  /* 0x1306 to 0x13ff undefined */
  {0x1400, "HDP"},
  {0x1401, "HDP Source"},
  {0x1402, "HDP Sink"},
  /* 0x1403 to 0x17ff undefined */
  {0x1800, "Generic Access Profile"},
  {0x1801, "Generic Attribute Profile"},
  {0x1802, "Immediate Alert"},
  {0x1803, "Link Loss"},
  {0x1804, "Tx Power"},
  {0x1805, "Current Time Service"},
  {0x1806, "Reference Time Update Service"},
  {0x1807, "Next DST Change Service"},
  {0x1808, "Glucose"},
  {0x1809, "Health Thermometer"},
  {0x180a, "Device Information"},
  /* 0x180b and 0x180c undefined */
  {0x180d, "Heart Rate"},
  {0x180e, "Phone Alert Status Service"},
  {0x180f, "Battery Service"},
  {0x1810, "Blood Pressure"},
  {0x1811, "Alert Notification Service"},
  {0x1812, "Human Interface Device"},
  {0x1813, "Scan Parameters"},
  {0x1814, "Running Speed and Cadence"},
  {0x1815, "Automation IO"},
  {0x1816, "Cycling Speed and Cadence"},
  /* 0x1817 undefined */
  {0x1818, "Cycling Power"},
  {0x1819, "Location and Navigation"},
  {0x181a, "Environmental Sensing"},
  {0x181b, "Body Composition"},
  {0x181c, "User Data"},
  {0x181d, "Weight Scale"},
  {0x181e, "Bond Management"},
  {0x181f, "Continuous Glucose Monitoring"},
  {0x1820, "Internet Protocol Support"},
  {0x1821, "Indoor Positioning"},
  {0x1822, "Pulse Oximeter"},
  {0x1823, "HTTP Proxy"},
  {0x1824, "Transport Discovery"},
  {0x1825, "Object Transfer"},
  /* 0x1824 to 0x27ff undefined */
  {0x2800, "Primary Service"},
  {0x2801, "Secondary Service"},
  {0x2802, "Include"},
  {0x2803, "Characteristic"},
  /* 0x2804 to 0x28ff undefined */
  {0x2900, "Characteristic Extended Properties"},
  {0x2901, "Characteristic User Description"},
  {0x2902, "Client Characteristic Configuration"},
  {0x2903, "Server Characteristic Configuration"},
  {0x2904, "Characteristic Format"},
  {0x2905, "Characteristic Aggregate Formate"},
  {0x2906, "Valid Range"},
  {0x2907, "External Report Reference"},
  {0x2908, "Report Reference"},
  {0x2909, "Number of Digitals"},
  {0x290a, "Value Trigger Setting"},
  {0x290b, "Environmental Sensing Configuration"},
  {0x290c, "Environmental Sensing Measurement"},
  {0x290d, "Environmental Sensing Trigger Setting"},
  {0x290e, "Time Trigger Setting"},
  /* 0x290f to 0x29ff undefined */
  {0x2a00, "Device Name"},
  {0x2a01, "Appearance"},
  {0x2a02, "Peripheral Privacy Flag"},
  {0x2a03, "Reconnection Address"},
  {0x2a04, "Peripheral Preferred Connection Parameters"},
  {0x2a05, "Service Changed"},
  {0x2a06, "Alert Level"},
  {0x2a07, "Tx Power Level"},
  {0x2a08, "Date Time"},
  {0x2a09, "Day of Week"},
  {0x2a0a, "Day Date Time"},
  /* 0x2a0b undefined */
  {0x2a0c, "Exact Time 256"},
  {0x2a0d, "DST Offset"},
  {0x2a0e, "Time Zone"},
  {0x2a0f, "Local Time Information"},
  /* 0x2a10 undefined */
  {0x2a11, "Time with DST"},
  {0x2a12, "Time Accuracy"},
  {0x2a13, "Time Source"},
  {0x2a14, "Reference Time Information"},
  /* 0x2a15 undefined */
  {0x2a16, "Time Update Control Point"},
  {0x2a17, "Time Update State"},
  {0x2a18, "Glucose Measurement"},
  {0x2a19, "Battery Level"},
  /* 0x2a1a and 0x2a1b undefined */
  {0x2a1c, "Temperature Measurement"},
  {0x2a1d, "Temperature Type"},
  {0x2a1e, "Intermediate Temperature"},
  /* 0x2a1f and 0x2a20 undefined */
  {0x2a21, "Measurement Interval"},
  {0x2a22, "Boot Keyboard Input Report"},
  {0x2a23, "System ID"},
  {0x2a24, "Model Number String"},
  {0x2a25, "Serial Number String"},
  {0x2a26, "Firmware Revision String"},
  {0x2a27, "Hardware Revision String"},
  {0x2a28, "Software Revision String"},
  {0x2a29, "Manufacturer Name String"},
  {0x2a2a, "IEEE 11073-20601 Regulatory Cert. Data List"},
  {0x2a2b, "Current Time"},
  {0x2a2c, "Magnetic Declination"},
  /* 0x2a2d to 0x2a30 undefined */
  {0x2a31, "Scan Refresh"},
  {0x2a32, "Boot Keyboard Output Report"},
  {0x2a33, "Boot Mouse Input Report"},
  {0x2a34, "Glucose Measurement Context"},
  {0x2a35, "Blood Pressure Measurement"},
  {0x2a36, "Intermediate Cuff Pressure"},
  {0x2a37, "Heart Rate Measurement"},
  {0x2a38, "Body Sensor Location"},
  {0x2a39, "Heart Rate Control Point"},
  /* 0x2a3a to 0x2a3e undefined */
  {0x2a3f, "Alert Status"},
  {0x2a40, "Ringer Control Point"},
  {0x2a41, "Ringer Setting"},
  {0x2a42, "Alert Category ID Bit Mask"},
  {0x2a43, "Alert Category ID"},
  {0x2a44, "Alert Notification Control Point"},
  {0x2a45, "Unread Alert Status"},
  {0x2a46, "New Alert"},
  {0x2a47, "Supported New Alert Category"},
  {0x2a48, "Supported Unread Alert Category"},
  {0x2a49, "Blood Pressure Feature"},
  {0x2a4a, "HID Information"},
  {0x2a4b, "Report Map"},
  {0x2a4c, "HID Control Point"},
  {0x2a4d, "Report"},
  {0x2a4e, "Protocol Mode"},
  {0x2a4f, "Scan Interval Window"},
  {0x2a50, "PnP ID"},
  {0x2a51, "Glucose Feature"},
  {0x2a52, "Record Access Control Point"},
  {0x2a53, "RSC Measurement"},
  {0x2a54, "RSC Feature"},
  {0x2a55, "SC Control Point"},
  {0x2a56, "Digital"},
  /* 0x2a57 undefined */
  {0x2a58, "Analog"},
  /* 0x2a59 undefined */
  {0x2a5a, "Aggregate"},
  {0x2a5b, "CSC Measurement"},
  {0x2a5c, "CSC Feature"},
  {0x2a5d, "Sensor Location"},
  /* 0x2a5e to 0x2a62 undefined */
  {0x2a63, "Cycling Power Measurement"},
  {0x2a64, "Cycling Power Vector"},
  {0x2a65, "Cycling Power Feature"},
  {0x2a66, "Cycling Power Control Point"},
  {0x2a67, "Location and Speed"},
  {0x2a68, "Navigation"},
  {0x2a69, "Position Quality"},
  {0x2a6a, "LN Feature"},
  {0x2a6b, "LN Control Point"},
  {0x2a6c, "Elevation"},
  {0x2a6d, "Pressure"},
  {0x2a6e, "Temperature"},
  {0x2a6f, "Humidity"},
  {0x2a70, "True Wind Speed"},
  {0x2a71, "True Wind Direction"},
  {0x2a72, "Apparent Wind Speed"},
  {0x2a73, "Apparent Wind Direction"},
  {0x2a74, "Gust Factor"},
  {0x2a75, "Pollen Concentration"},
  {0x2a76, "UV Index"},
  {0x2a77, "Irradiance"},
  {0x2a78, "Rainfall"},
  {0x2a79, "Wind Chill"},
  {0x2a7a, "Heat Index"},
  {0x2a7b, "Dew Point"},
  {0x2a7c, "Trend"},
  {0x2a7d, "Descriptor Value Changed"},
  {0x2a7e, "Aerobic Heart Rate Lower Limit"},
  {0x2a7f, "Aerobic Threshold"},
  {0x2a80, "Age"},
  {0x2a81, "Anaerobic Heart Rate Lower Limit"},
  {0x2a82, "Anaerobic Heart Rate Upper Limit"},
  {0x2a83, "Anaerobic Threshold"},
  {0x2a84, "Aerobic Heart Rate Upper Limit"},
  {0x2a85, "Date of Birth"},
  {0x2a86, "Date of Threshold Assessment"},
  {0x2a87, "Email Address"},
  {0x2a88, "Fat Burn Heart Rate Lower Limit"},
  {0x2a89, "Fat Burn Heart Rate Upper Limit"},
  {0x2a8a, "First Name"},
  {0x2a8b, "Five Zone Heart Rate Limits"},
  {0x2a8c, "Gender"},
  {0x2a8d, "Heart Rate Max"},
  {0x2a8e, "Height"},
  {0x2a8f, "Hip Circumference"},
  {0x2a90, "Last Name"},
  {0x2a91, "Maximum Recommended Heart Rate"},
  {0x2a92, "Resting Heart Rate"},
  {0x2a93, "Sport Type for Aerobic/Anaerobic Thresholds"},
  {0x2a94, "Three Zone Heart Rate Limits"},
  {0x2a95, "Two Zone Heart Rate Limit"},
  {0x2a96, "VO2 Max"},
  {0x2a97, "Waist Circumference"},
  {0x2a98, "Weight"},
  {0x2a99, "Database Change Increment"},
  {0x2a9a, "User Index"},
  {0x2a9b, "Body Composition Feature"},
  {0x2a9c, "Body Composition Measurement"},
  {0x2a9d, "Weight Measurement"},
  {0x2a9e, "Weight Scale Feature"},
  {0x2a9f, "User Control Point"},
  {0x2aa0, "Magnetic Flux Density - 2D"},
  {0x2aa1, "Magnetic Flux Density - 3D"},
  {0x2aa2, "Language"},
  {0x2aa3, "Barometric Pressure Trend"},
  {0x2aa4, "Bond Management Control Point"},
  {0x2aa5, "Bond Management Feature"},
  {0x2aa6, "Central Address Resolution"},
  {0x2aa7, "CGM Measurement"},
  {0x2aa8, "CGM Feature"},
  {0x2aa9, "CGM Status"},
  {0x2aaa, "CGM Session Start Time"},
  {0x2aab, "CGM Session Run Time"},
  {0x2aac, "CGM Specific Ops Control Point"},
  {0x2aad, "Indoor Positioning Configuration"},
  {0x2aae, "Latitude"},
  {0x2aaf, "Longitude"},
  {0x2ab0, "Local North Coordinate"},
  {0x2ab1, "Local East Coordinate"},
  {0x2ab2, "Floor Number"},
  {0x2ab3, "Altitude"},
  {0x2ab4, "Uncertainty"},
  {0x2ab5, "Location Name"},
  {0x2ab6, "URI"},
  {0x2ab7, "HTTP Headers"},
  {0x2ab8, "HTTP Status Code"},
  {0x2ab9, "HTTP Entity Body"},
  {0x2aba, "HTTP Control Point"},
  {0x2abb, "HTTPS Security"},
  {0x2abc, "TDS Control Point"},
  {0x2abd, "OTS Feature"},
  {0x2abe, "Object Name"},
  {0x2abf, "Object Type"},
  {0x2ac0, "Object Size"},
  {0x2ac1, "Object First-Created"},
  {0x2ac2, "Object Last-Modified"},
  {0x2ac3, "Object ID"},
  {0x2ac4, "Object Properties"},
  {0x2ac5, "Object Action Control Point"},
  {0x2ac6, "Object List Control Point"},
  {0x2ac7, "Object List Filter"},
  {0x2ac8, "Object Changed"},
  /* vendor defined */
  {0xfeff, "GN Netcom"},
  {0xfefe, "GN ReSound A/S"},
  {0xfefd, "Gimbal, Inc."},
  {0xfefc, "Gimbal, Inc."},
  {0xfefb, "Stollmann E+V GmbH"},
  {0xfefa, "PayPal, Inc."},
  {0xfef9, "PayPal, Inc."},
  {0xfef8, "Aplix Corporation"},
  {0xfef7, "Aplix Corporation"},
  {0xfef6, "Wicentric, Inc."},
  {0xfef5, "Dialog Semiconductor GmbH"},
  {0xfef4, "Google"},
  {0xfef3, "Google"},
  {0xfef2, "CSR"},
  {0xfef1, "CSR"},
  {0xfef0, "Intel"},
  {0xfeef, "Polar Electro Oy"},
  {0xfeee, "Polar Electro Oy"},
  {0xfeed, "Tile, Inc."},
  {0xfeec, "Tile, Inc."},
  {0xfeeb, "Swirl Networks, Inc."},
  {0xfeea, "Swirl Networks, Inc."},
  {0xfee9, "Quintic Corp."},
  {0xfee8, "Quintic Corp."},
  {0xfee7, "Tencent Holdings Limited"},
  {0xfee6, "Seed Labs, Inc."},
  {0xfee5, "Nordic Semiconductor ASA"},
  {0xfee4, "Nordic Semiconductor ASA"},
  {0xfee3, "Anki, Inc."},
  {0xfee2, "Anki, Inc."},
  {0xfee1, "Anhui Huami Information Technology Co."},
  {0xfee0, "Anhui Huami Information Technology Co."},
  {0xfedf, "Design SHIFT"},
  {0xfede, "Coin, Inc."},
  {0xfedd, "Jawbone"},
  {0xfedc, "Jawbone"},
  {0xfedb, "Perka, Inc."},
  {0xfeda, "ISSC Technologies Corporation"},
  {0xfed9, "Pebble Technology Corporation"},
  {0xfed8, "Google"},
  {0xfed7, "Broadcom Corporation"},
  {0xfed6, "Broadcom Corporation"},
  {0xfed5, "Plantronics Inc."},
  {0xfed4, "Apple, Inc."},
  {0xfed3, "Apple, Inc."},
  {0xfed2, "Apple, Inc."},
  {0xfed1, "Apple, Inc."},
  {0xfed0, "Apple, Inc."},
  {0xfecf, "Apple, Inc."},
  {0xfece, "Apple, Inc."},
  {0xfecd, "Apple, Inc."},
  {0xfecc, "Apple, Inc."},
  {0xfecb, "Apple, Inc."},
  {0xfeca, "Apple, Inc."},
  {0xfec9, "Apple, Inc."},
  {0xfec8, "Apple, Inc."},
  {0xfec7, "Apple, Inc."},
  {0xfec6, "Kocomojo, LLC"},
  {0xfec5, "Realtek Semiconductor Corp."},
  {0xfec4, "PLUS Location Systems"},
  {0xfec3, "360fly, Inc."},
  {0xfec2, "Blue Spark Technologies, Inc."},
  {0xfec1, "KDDI Corporation"},
  {0xfec0, "KDDI Corporation"},
  {0xfebf, "Nod, Inc."},
  {0xfebe, "Bose Corporation"},
  {0xfebd, "Clover Network, Inc."},
  {0xfebc, "Dexcom, Inc."},
  {0xfebb, "adafruit industries"},
  {0xfeba, "Tencent Holdings Limited"},
  {0xfeb9, "LG Electronics"},
  {0xfeb8, "Facebook, Inc."},
  {0xfeb7, "Facebook, Inc."},
  {0xfeb6, "Vencer Co, Ltd"},
  {0xfeb5, "WiSilica Inc."},
  {0xfeb4, "WiSilica Inc."},
  {0xfeb3, "Taobao"},
  {0xfeb2, "Microsoft Corporation"},
  {0xfeb1, "Electronics Tomorrow Limited"},
  {0xfeb0, "Nest Labs Inc."},
  {0xfeaf, "Nest Labs Inc."},
  {0xfeae, "Nokia Corporation"},
  {0xfead, "Nokia Corporation"},
  {0xfeac, "Nokia Corporation"},
  {0xfeab, "Nokia Corporation"},
  {0xfeaa, "Google"},
  {0xfea9, "Savant Systems LLC"},
  {0xfea8, "Savant Systems LLC"},
  {0xfea7, "UTC Fire and Security"},
  {0xfea6, "GoPro, Inc."},
  {0xfea5, "GoPro, Inc."},
  {0xfea4, "Paxton Access Ltd"},
  {0xfea3, "ITT Industries"},
  {0xfea2, "Intrepid Control Systems, Inc."},
  {0xfea1, "Intrepid Control Systems, Inc."},
  {0xfea0, "Google"},
  {0xfe9f, "Google"},
  {0xfe9e, "Dialog Semiconductor B.V."},
  {0xfe9d, "Mobiquity Networks Inc"},
  {0xfe9c, "GSI Laboratories, Inc."},
  {0xfe9b, "Samsara Networks, Inc"},
  {0xfe9a, "Estimote"},
  {0xfe99, "Currant, Inc."},
  {0xfe98, "Currant, Inc."},
  {0xfe97, "Tesla Motor Inc."},
  {0xfe96, "Tesla Motor Inc."},
  {0xfe95, "Xiaomi Inc."},
  {0xfe94, "OttoQ Inc."},
  {0xfe93, "OttoQ Inc."},
  {0xfe92, "Jarden Safety & Security"},
  {0xfe91, "Shanghai Imilab Technology Co.,Ltd"},
  {0xfe90, "JUMA"},
  {0xfe8f, "CSR"},
  {0xfe8e, "ARM Ltd"},
  {0xfe8d, "Interaxon Inc."},
  {0xfe8c, "TRON Forum"},
  {0xfe8b, "Apple, Inc."},
  {0xfe8a, "Apple, Inc."},
  {0xfe89, "B&O Play A/S"},
  {0xfe88, "SALTO SYSTEMS S.L."},
  {0xfe87,
   "Qingdao Yeelink Information Technology Co., Ltd. ( 青岛亿联客信息技术有限公司 )"},
  {0xfe86, "HUAWEI Technologies Co., Ltd. ( 华为技术有限公司 )"},
  {0xfe85, "RF Digital Corp"},
  {0xfe84, "RF Digital Corp"},
  {0xfe83, "Blue Bite"},
  {0xfe82, "Medtronic Inc."},
  {0xfe81, "Medtronic Inc."},
  {0xfe80, "Doppler Lab"},
  {0xfe7f, "Doppler Lab"},
  {0xfe7e, "Awear Solutions Ltd"},
  {0xfe7d, "Aterica Health Inc."},
  {0xfe7c, "Stollmann E+V GmbH"},
  {0xfe7b, "Orion Labs, Inc."},
  /* SDO defined */
  {0xfffe, "Alliance for Wireless Power (A4WP)"},
  {0xfffd, "Fast IDentity Online Alliance (FIDO)"},
  /*TI sensor Tag defined */
  {0xAA01, "IR Temperature Data"},
  {0xAA02, "IR Temperature Config"},
  {0xAA03, "IR Temperature Period"},
  {0xAA11, "Accelerometer Data"},
  {0xAA12, "Accelerometer Config"},
  {0xAA13, "Accelerometer Period"},
  {0xAA21, "Humidity Data"},
  {0xAA22, "Humidity Config"},
  {0xAA23, "Humidity Period"},
  {0xAA31, "Magnetometer Data"},
  {0xAA32, "Magnetometer Config"},
  {0xAA33, "Magnetometer Period"},
  {0xAA41, "Barometer Data"},
  {0xAA42, "Barometer Configuration"},
  {0xAA44, "Barometer period"},
  {0xAA43, "Barometer Calibration"},
  {0xAA51, "Gyroscope Data"},
  {0xAA52, "Gyroscope Config"},
  {0xAA53, "Gyroscope Period"},
  {0xAA61, "Test Data"},
  {0xAA62, "Test Config"},
  {0xCCC1, "Connection Parameters"},
  {0xCCC2, "Request Connection Parameters"},
  {0xCCC3, "Disconnect request"},
  {0xFFC1, "OAD Image Identify"},
  {0xFFC2, "OAD Image Block"},
  {0xFFE1, "simple key service"},
  {}
};


key_value_pair_t1 *pKeyValue_Global = NULL;

info characteristic_table[MAX_SIZE];
static bool verbose = false;
int write_done;
pthread_t tid;
char address_global[18];
BLE_Device_InfoTable_t BLE_Device_InfoTable;

static int table_index = 0;

void write_to_table (int fd, char start[], char stop[])
{
  unsigned char wpdu[512];
  unsigned char rpdu[512];
  unsigned int len = 0;
  unsigned int i = 0, j = 0;
  unsigned int size = 0;
  int temp_index = 0;
  uint16_t last_handle;
  wpdu[0] = BT_ATT_OP_FIND_INFO_REQ;
  wpdu[1] = start[1];
  wpdu[2] = start[0];
  wpdu[3] = stop[1];
  wpdu[4] = stop[0];
  last_handle = start[1] | (start[0] << 8);
  len = 0;
  while (last_handle != 0xffff)
    {
      wpdu[1] = last_handle & 0xff;
      wpdu[2] = (last_handle & 0xff00) >> 8;
      size = write (fd, wpdu, 5);
      len = read (fd, rpdu, 512);
      if (rpdu[0] != BT_ATT_OP_ERROR_RSP)
    {

      if (rpdu[1] == 0x01)
        {
          for (i = 0; i < ((len - 2) / 4); i++)
        {
          j = (4 * i) + 2;
          characteristic_table[table_index].handle = (rpdu[j + 1] << 8) | rpdu[j + 0];
          characteristic_table[table_index].type[13] = rpdu[j + 3];
          characteristic_table[table_index].type[12] = rpdu[j + 2];
          last_handle = characteristic_table[table_index].handle + 1;
          table_index++;
        }
        }
      else if (rpdu[1] == 0x02)
        {
          for (i = 0; i < ((len - 2) / 18); i++)
        {
          j = (i * 18) + 2;
          characteristic_table[table_index].handle = rpdu[j + 1] << 8 | rpdu[j + 0];
          memcpy (characteristic_table[table_index].type,&rpdu[j + 2], 16);
          last_handle = characteristic_table[table_index].handle + 1;
          table_index++;
        }
        }
    }
      else
    {
      return;
    }
    }
  return;
}

int get_table_index_by_uuid (uint16_t hnd, int index_val)
{
  int i;
  if(index_val < table_index)
  {
    for (i = index_val; i < table_index; i++)
        {
            if ((characteristic_table[i].type[12] == (hnd & 0xff)) && (characteristic_table[i].type[13] == ((hnd >> 8) & 0xff)))
            {
                return i;
            }
    }
  }
  return 0xffff;
}

int get_index_by_handle (uint16_t hnd)
{
  int i;
  for (i = 0; i < table_index; i++)
    {
        if (characteristic_table[i].handle == hnd)
            return i;
    }
  return 0xffff;
}

char *uuid_to_str (uint16_t uuid)
{
  int i;
  for (i = 0; i < (sizeof (uuid_table) / sizeof (uuid_table_t)); i++)
    {
      if (uuid_table[i].uuid == uuid)
    return uuid_table[i].str;
    }

  return "Unknown";
}

key_value_pair_t1 *form_key_value_pair (info * tbl)
{
  int index = 0;
  int i;
  int primary_index;
  int nCharIndex;
  int key_value_pair_index;
  key_value_pair_t1 *pKeyValue_LocalNode;
  key_value_pair_t1 *pKeyValue_Temp;
  unsigned char *pcKey;
  uint16_t handle_result;
  uint16_t test_uuid;
  while ((primary_index = get_table_index_by_uuid (0x2800, index)) != 0xffff)
    {
      nCharIndex = get_table_index_by_uuid (0x2803, primary_index);
      handle_result = (tbl[nCharIndex].value[3]<<8)& 0Xff| (tbl[nCharIndex].value[2] & 0Xff);
      key_value_pair_index = get_index_by_handle (handle_result);
      if(key_value_pair_index == 0xffff)
      {
         printf("index not found for handle : %4.x\n", handle_result);
         index = nCharIndex;
         continue;
      }
      pKeyValue_LocalNode = malloc (sizeof (key_value_pair_t1));
      pKeyValue_LocalNode->pNext = NULL;
      test_uuid = tbl[key_value_pair_index].type[13] << 8 | tbl[key_value_pair_index].type[12];
      pcKey = uuid_to_str (test_uuid);

      strcpy (pKeyValue_LocalNode->key, pcKey);
      memcpy (pKeyValue_LocalNode->value, tbl[key_value_pair_index].value, 512);
      printf("key = %s and value = %s\n ",pKeyValue_LocalNode->key,pKeyValue_LocalNode->value);

      pKeyValue_LocalNode->handle_res = tbl[key_value_pair_index].handle;
      pKeyValue_Temp = pKeyValue_Global;
      if (pKeyValue_Global == NULL)
    {
          pKeyValue_Global = pKeyValue_LocalNode;
    }
      else
    {
      while (pKeyValue_Temp->pNext != NULL)
        {
          pKeyValue_Temp = pKeyValue_Temp->pNext;
        }
          pKeyValue_Temp->pNext = pKeyValue_LocalNode;
    }
       index = nCharIndex;
    }
      return pKeyValue_Global;
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

    printf(" Done\n");

    return sock;
}

void *read_data_non(void *pfd)
{
    printf("*******************%s\n",__func__);
    unsigned char rpdu[512];
    int len=0,temp;
    long int fd=(long int )pfd;

    while(1)
    {
         len = read(fd,rpdu,512);
         printf("************length %d data byte %0.2x\n",len,rpdu[0]);

            if(len != -1)
            {
                if(rpdu[0] == 0x1B)
                {
                   // CALEAdapterClientReceivedDataEx((const char *)"00:1A:7D:DA:71:13",(const uint8_t *)rpdu+3,len-3,&temp);
                    //printf("************length %d\n",len);
                }//wpdu
            }//len
            else
               sleep(1);

    }//while
}

void init_non (long int fd)
{
     printf("*******************%s\n",__func__);
  int error;
  error=pthread_create(&tid, NULL,read_data_non,(void *)fd);
  if(error!=0)
     printf("thread creation failed\n");
  return;
}
void mtu_request_non(int fd)
{
     printf("*******************%s\n",__func__);
    unsigned char wpdu[512];
    unsigned char rpdu[512];

    unsigned int size=0;

    wpdu[0] = BT_ATT_OP_MTU_REQ;
    wpdu[1]=0x00;
    wpdu[2]=0x01;

    size=write(fd,wpdu,3);
    sleep(1);
    read(fd,rpdu,512);

}

int connect_iot_dm_non(char* dst_address)
{
    printf("******************* %s\n",__func__);
        int sec = BT_SECURITY_LOW;
        uint8_t dst_type = BDADDR_LE_PUBLIC;
        bdaddr_t src_addr, dst_addr;
        int dev_id = -1;
        int fd;
        printf("*********************AAAdress %s\n",dst_address);
        //strcpy(dst_address, "90:59:AF:0B:7F:D4");
        if (str2ba(dst_address, &dst_addr) < 0)
        {
         perror("Invalid remote address:\n");
         return EXIT_FAILURE;
                }

        if (dev_id == -1)
            bacpy(&src_addr, BDADDR_ANY);
        else if (hci_devba(dev_id, &src_addr) < 0)
        {
            perror("Adapter not available");
            return EXIT_FAILURE;
        }

        fd = l2cap_le_att_connect(&src_addr, &dst_addr, dst_type, sec);
        if(fd<0)
        {
            return EXIT_FAILURE;
        }
        write_done=0;
        mtu_request_non(fd);
       // init(fd);
        enable_notification_non(fd);

        printf("****************file descriptor %d\n",fd);
       // while(1);
        return fd;
}
//*******************VITA_DM******************************

void VITA_WRITE_REQUEST_non(int file_descriptor,  uint8_t *data,  uint8_t length)
{
    unsigned char rpdu[ 512 ];
    int fd=file_descriptor;
    printf("VITA_WRITE_REQUEST_non ==>fd: %d Length:%d \n",file_descriptor,length);
    uint8_t len;
    for(int i=0;i<length;i++)
        printf(" %0.2x ",data[i]);
    printf("\n");
    len = write( file_descriptor, data, length );
    printf("write length %d\n",len);
    if(len != length)
    {
        printf(" Write Failure\n");
        printf("fd %d\n",fd);
        close(fd);
       connect_iot_dm_non("00:1A:7D:DA:71:13");
        printf("fd %d\n",fd);
        len = write(fd, data, length );
    }
    sleep(1);
    while( 1 )
    {
        printf("in loop\n");
    len =read(file_descriptor,rpdu,512);
 //   if(len !=-1 && rpdu[0] == 0x13 )
        break;
    }
    return;
}


void enable_notification_non(int fd)
{
     printf("*******************%s\n",__func__);
    unsigned char wpdu[512];
    unsigned int size=0;
    wpdu[0] = BT_ATT_OP_WRITE_REQ;
    wpdu[1]=0x10;
    wpdu[2]=0x00;
    wpdu[3]=0x01;
    wpdu[4]=0x00;

    VITA_WRITE_REQUEST_non(fd,wpdu,5);
    sleep(1);
}

void NonIoTivity_Init (void)
{
    printf("NonIoTivity_Init============================> \n");
  memset (&BLE_Device_InfoTable, 0, sizeof (BLE_Device_InfoTable));
  BLE_Device_InfoTable.ucNumOfDevices = 0;
  printf("NonIoTivity_Init============================>out \n");
}

unsigned char* NonIoTivity_AddDevice (unsigned char *ucpMacId,unsigned char *ucpDeviceName)
{
    printf("************** ADD DEVICE ************* \n");

    static int index=0;//BLE_Device_InfoTable.ucNumOfDevices;

    char Non_Iot_dev_name[100];

    printf("mac id=%s  devicename=%s\n",ucpMacId,ucpDeviceName);

    if(ucpDeviceName!=NULL && ucpMacId !=NULL)
    {
        memcpy(BLE_Device_InfoTable.BLE_DeviceInfo[index].ucMacId,ucpMacId,strlen(ucpMacId));	// Get from Advertisement packet
        memcpy(BLE_Device_InfoTable.BLE_DeviceInfo[index].ucDeviceName,ucpDeviceName,strlen((ucpDeviceName)));	// Get from Advertisement packet

        sprintf(Non_Iot_dev_name,"/non_iotvity-%s",ucpMacId);
        strcpy(BLE_Device_InfoTable.BLE_DeviceInfo[index].ucURI,Non_Iot_dev_name); // DeviceName;	// Get from Advertisement packet
        BLE_Device_InfoTable.BLE_DeviceInfo[index].eState = DISCOVERED;
        BLE_Device_InfoTable.ucNumOfDevices++;
        index++;


       recievePacketNotify(Non_Iot_dev_name);

        return Non_Iot_dev_name;

        //Call Ganesh's discovery funtion (BLE_Device_InfoTable.BLE_DeviceInfo[index].ucURI);
    }
}

void read_by_handle_to_value (int fd, info table_entry[])
{
  unsigned char wpdu[512];
  unsigned char rpdu[512];
  unsigned int len = 0;
  unsigned int i;
  unsigned int size = 0;
  wpdu[0] = BT_ATT_OP_READ_REQ;
  int j;
  for (j = 0; j < table_index; j++)
    {
      wpdu[1] = table_entry[j].handle & 0xff;
      wpdu[2] = (table_entry[j].handle & 0xff00) >> 8;
      size = write (fd, wpdu, 3);
      len = read (fd, rpdu, 512);
      if(len != -1)
      {
          memset (table_entry[j].value, 0, sizeof (table_entry[j].value));
          memcpy(table_entry[j].value,rpdu,len);
      }
     }
}


key_value_pair_t1* NonIoTivity_Get (unsigned char* URI)
{

    char start[2] = { 0x00, 0x01 };
    char end[2] = { 0xff, 0xff };
    key_value_pair_t1 *pairs;
    int fd;
    bool result;
    printf("AShok code URI name %s\n",URI);
    printf("NonIoTivity_Get============================>1 \n");
    int i;
    for (i = 0; i < BLE_Device_InfoTable.ucNumOfDevices; i++)
    {
        //printf("NonIoTivity_Get============================>2 \n");
        if (!strcmp(BLE_Device_InfoTable.BLE_DeviceInfo[i].ucURI,URI))
        {
            break;
        }
    }
    printf("NonIoTivity_Get============================>3 \n");
    if (i == BLE_Device_InfoTable.ucNumOfDevices)
    {
        //printf("NonIoTivity_Get============================>4 \n");
        perror("We don \'t have any device matching URI\n");
        return;
    }
    //printf("NonIoTivity_Get============================>5  i=%d \n",i);
    // We found device at "i"

   // char *macId = "90:59:AF:0B:7F:D4";
    fd=connect_iot_dm_non(BLE_Device_InfoTable.BLE_DeviceInfo[i].ucMacId);

   // printf("NonIoTivity_Get============================>5A i=%d \n",i);

    if (result < 0)
        return EXIT_FAILURE;

    write_to_table (fd, start, end);
   // printf("NonIoTivity_Get============================>5B i=%d \n",i);

    read_by_handle_to_value (fd, characteristic_table);
   // printf("NonIoTivity_Get============================>5c i=%d \n",i);

    pairs = form_key_value_pair(characteristic_table);
   // printf("NonIoTivity_Get============================>5D i=%d \n",i);

    //read_all_characteristics values;
    BLE_Device_InfoTable.BLE_DeviceInfo[i].pJSON=pairs;
//    strcpy(BLE_Device_InfoTable.BLE_DeviceInfo[i].pJSON->obj.key,"Name");
//    printf("NonIoTivity_Get============================>5 BLE_Device_InfoTable.BLE_DeviceInfo[0].pJSON->obj.key=%s\n",BLE_Device_InfoTable.BLE_DeviceInfo[i].pJSON->obj.key);
//    strcpy(BLE_Device_InfoTable.BLE_DeviceInfo[i].pJSON->obj.value,"Sensor Tag");
//    printf("NonIoTivity_Get============================>5 BLE_Device_InfoTable.BLE_DeviceInfo[0].pJSON->obj.key=%s\n",BLE_Device_InfoTable.BLE_DeviceInfo[i].pJSON->obj.value);

    printf("NonIoTivity_Get============================>6 \n");

    key_value_pair_t1 *tempJson = NULL;


    tempJson = BLE_Device_InfoTable.BLE_DeviceInfo[i].pJSON;

    testGetdata = BLE_Device_InfoTable.BLE_DeviceInfo[i].pJSON;

//    printf("NONIOTVITY _ going to print json values which are stored in address: %.8x\n", tempJson);
//    while(tempJson->pNext != NULL)
//    {
//        printf("NONIOTVITY _  address of data in get method = %s \n",tempJson->key);
//        printf("NONIOTVITY _ address of data next in get method = %s \n",tempJson->value);
//        tempJson = tempJson->pNext;
//    }

    close(fd);
    return BLE_Device_InfoTable.BLE_DeviceInfo[i].pJSON;
}


void NonIoTivity_Put (unsigned char* URI, key_value_pair_t1 *json)
{

    printf("NonIoTivity_Put ==> 1\n");

    key_value_pair_t1 *GetJsonOut;
    int index=0;
    int fd;
    unsigned char wpdu[512];
    key_value_pair_t1 *Tempjson=json;
    key_value_pair_t1 *TempGetJsonOut;

    Tempjson=json;
    GetJsonOut=NonIoTivity_Get (URI);


    for (index = 0; index < BLE_Device_InfoTable.ucNumOfDevices; index++)
        if (strcmp(BLE_Device_InfoTable.BLE_DeviceInfo[index].ucURI,URI)==0)
           break;
    if (index == BLE_Device_InfoTable.ucNumOfDevices)
    {
        perror("We don \'t have any device matching URI for PUT\n");
        printf("We don \'t have any device matching URI for PUT\n");
        return;
    }
    printf("NonIoTivity_Put ==> Connecting to [%d] %s\n",
           index,BLE_Device_InfoTable.BLE_DeviceInfo[index].ucMacId );

   // char *macId = "90:59:AF:0B:7F:D4";
    fd=connect_iot_dm_non(BLE_Device_InfoTable.BLE_DeviceInfo[index].ucMacId);
    if(fd < 0)
    {
        printf("NonIoTivity_Put ==> Error at connect_iot_dm_non\n");
        return;
    }

    while(Tempjson)// !=NULL)
    {
        printf("WRITTING==PUT FUNCTION============================2 \n");
        TempGetJsonOut=GetJsonOut;
        while(TempGetJsonOut != NULL)
        {
            printf("WRITTING==PUT FUNCTION============================3 \nTempjson->key=%s TempGetJsonOut->key=%s\n",Tempjson->key,TempGetJsonOut->key);
            if(strcmp(Tempjson->key,TempGetJsonOut->key)==0)
            {
                int len;
                printf("WRITTING==PUT FUNCTION============================4 \n");
                int value_length;
                wpdu[0] = BT_ATT_OP_WRITE_REQ;
                wpdu[1] = TempGetJsonOut->handle_res & 0xff;
                wpdu[2] = TempGetJsonOut->handle_res<<8;

                value_length = strlen(Tempjson->value);
                memcpy(&wpdu[3],Tempjson->value, value_length);
                len = write(fd,wpdu,value_length+3);
                if(len == value_length+3)
                    printf("NonIoTivity_Put: Successs \n");
                break;

            }
//               strcpy(BLE_Device_InfoTable.BLE_DeviceInfo[index].pJSON->value,json->value);
            TempGetJsonOut=TempGetJsonOut->pNext;
            printf("WRITTING==PUT FUNCTION============================5 \n");
        }
        Tempjson=Tempjson->pNext;
            printf("WRITTING==PUT FUNCTION============================6 \n");
    }
    printf("WRITTING==PUT FUNCTION============================7 \n");
    close(fd);

}
