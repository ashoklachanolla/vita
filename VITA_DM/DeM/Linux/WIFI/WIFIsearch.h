#ifndef CONFIGURE_WIFI_H
#define CONFIGURE_WIFI_H

#include "VITA_DM_INTERNAL.h"

#ifdef __cplusplus
extern "C" {
#endif
int DM_Wifi_Scan(DM_WIFI_ScanResult_Info_t *resultData);
#ifdef __cplusplus
}
#endif

#endif //CONFIGURE_WIFI_H
