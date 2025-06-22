#include "WifiStationMode.hpp"
#include "../tcp/TcpServerClass.hpp"

using namespace std;

const string WifiStationMode::TAG = "WifiStationMode";

#define CYP_LOG(fmt, ...) ESP_LOGI((WifiStationMode::TAG).c_str(), fmt, ##__VA_ARGS__)

void WifiStationMode::event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{

    WifiStationMode* self = static_cast<WifiStationMode*>(arg);

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        self->isConnectedToWifi = false;
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        self->isConnectedToWifi = false;
        esp_wifi_connect();
        CYP_LOG("retry to connect to the AP");           
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        CYP_LOG("got ip:" IPSTR, IP2STR(&event->ip_info.ip));

        self->isConnectedToWifi = true;
        if(!(self->all_sockets_init)){
            
            self->all_sockets_init = true;
            // Enable TCP Server
            xSemaphoreGive(TcpServerClass::tcpServerSemaphore);
        }    
        
        xEventGroupSetBits(self->s_wifi_event_group, WIFI_CONNECTED_BIT);
        
    }
}

void WifiStationMode::wifi_init_sta()
{

    wifiInfoClass = new WifiInfoClass();
    wifiInfoClass->init_mac_address();

    CYP_LOG("ESP_WIFI_MODE_STA");

    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifiInfoClass->p_netif = esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &(WifiStationMode::event_handler),
                                                        this,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &(WifiStationMode::event_handler),
                                                        this,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {};

    strncpy((char *) wifi_config.sta.ssid, WifiInfoClass::ROUTER_SSID.c_str() , 256);
    strncpy((char *) wifi_config.sta.password, WifiInfoClass::ROUTER_PASS.c_str() , 256); 

    if (strlen((char *)wifi_config.sta.password)) {
        wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    }

    wifi_config.sta.pmf_cfg.capable = true;
    wifi_config.sta.pmf_cfg.required = false;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    CYP_LOG("wifi_init_sta finished.");

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT) {
        CYP_LOG("connected to AP");
        isConnectedToWifi = true;
    } else if (bits & WIFI_FAIL_BIT) {
        CYP_LOG("Failed to connect to AP");
        isConnectedToWifi = false;
    } else {
        CYP_LOG("UNEXPECTED EVENT");
        isConnectedToWifi = false;
    }

    /* The event will not be processed after unregister */
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id));
    vEventGroupDelete(s_wifi_event_group);

    vTaskDelete(NULL);
}

void WifiStationMode::task_entry(void *pvParamters){
    WifiStationMode *self = static_cast<WifiStationMode*>(pvParamters);
    self->run();
}

void WifiStationMode::run(){
    this->wifi_init_sta();
}

void WifiStationMode::start(){

    xTaskCreate(WifiStationMode::task_entry, "wifi_init_sta_task", 20480, this, 5, NULL);
}