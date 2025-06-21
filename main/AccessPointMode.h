// #include <string.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "esp_system.h"
// #include "esp_wifi.h"
#include "esp_event.h"
// #include "esp_log.h"
// #include "nvs_flash.h"

// #include "lwip/err.h"
// #include "lwip/sys.h"
// #include <stdio.h>
// #include "freertos/FreeRTOS.h"
// #include "constants.h"
// #include "params.h"
// #include "wifi_info_handler.h"

void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
void wifi_init_accesspoint_mode();
void update_wifi_ip_ap();
