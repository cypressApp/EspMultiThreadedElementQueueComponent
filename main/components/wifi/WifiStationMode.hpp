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

    bool all_sockets_init;

    /**
     * @brief handle wifi connection events
     * 
     * This function is called when the wifi connection status changes (e.g. connected, disconnected, ...) 
     *  
     */

    static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
    
    /**
     * @brief configure the device to station mode 
     * 
     * In station mode the device tries to connect to the router.
     * the router cridention should be done in configuration. (ROUTER_SSID, ROUTER_PASS)
     * 
     */
    void wifi_init_sta();

    /**
     * @brief task entity
     * 
     * This is an static function to run freeRTOS task in c++.
     * 
     */
    static void task_entry(void *pvParamters);

    /**
     * @brief run task
     * 
     * freeRTOS task runs this function.
     * 
     */
    void run();
    /**
     * @brief task initialization
     * 
     * This function initializes freeRTOS task in this function. 
     * 
     */
    void start();
};