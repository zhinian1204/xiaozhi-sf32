#ifndef XIAOZHI_PUBLIC_H
#define XIAOZHI_PUBLIC_H

#include <rtthread.h>
#include "lwip/apps/mqtt.h"
#include "lwip/apps/websocket_client.h"
#include "cJSON.h"

// 可复用函数
char *get_mac_address(void);
void hash_run(uint8_t algo, uint8_t *raw_data, uint32_t raw_data_len,
              uint8_t *result, uint32_t result_len);
void hex_2_asc(uint8_t n, char *str);
char *get_client_id();
int check_internet_access();
char *get_xiaozhi();
char *my_json_string(cJSON *json, char *key);

#endif // XIAOZHI_PUBLIC_H