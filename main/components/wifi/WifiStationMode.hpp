#include "WifiInfoClass.hpp"

extern "C" {

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "freertos/semphr.h" 
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

}

class WifiStationMode{

    static const auto ESP_MAXIMUM_RETRY = 10;
    static const auto WIFI_CONNECTED_BIT = BIT0;
    static const auto WIFI_FAIL_BIT = BIT1;
    static const std::string TAG;

    EventGroupHandle_t s_wifi_event_group;
    bool isConnectedToWifi ;
    WifiInfoClass *wifiInfoClass;

public:

    int s_retry_num;
    bool all_sockets_init;
    char ssid_arg[128];
    char pass_arg[128];

    static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
    void update_wifi_ip_sta();
    void wifi_init_sta();

    static void task_entry(void *pvParamters);
    void run();
    void start();
};