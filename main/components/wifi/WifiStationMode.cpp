#include "WifiStationMode.hpp"

// EventGroupHandle_t s_wifi_event_group;

// int  s_retry_num      = 0    ;
// bool all_sockets_init = false;
// char ssid_arg[128];
// char pass_arg[128];

// bool isConnectedToWifi = false;
// int xre4 = 0;

#define TAG "WifiStationMode"

#define CYP_LOG(fmt, ...) ESP_LOGI(TAG, fmt, ##__VA_ARGS__)

void WifiStationMode::event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{

    WifiStationMode* self = static_cast<WifiStationMode*>(arg);

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        self->isConnectedToWifi = false;
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        self->isConnectedToWifi = false;
        // if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
             esp_wifi_connect();
        //     s_retry_num++;
        CYP_LOG("retry to connect to the AP");           
        // } else {
        //     xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        // }
        CYP_LOG("connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        CYP_LOG("got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        // p_netif = *(&event->esp_netif);

#ifdef ENABLE_WIFI_STATIC_IP        
        if (esp_netif_dhcpc_stop(event->esp_netif) != ESP_OK) {

            isConnectedToWifi = true;
            if(!all_sockets_init){
                
                all_sockets_init = true;
                // Enable UDP Server
                xTaskCreate(udp_server_task, "udp_server", UDP_SERVER_TASK_STACK_DEPTH , (void*)AF_INET, 5, NULL);
                // Enable TCP Server
                xTaskCreate(tcp_server_task, "tcp_server", TCP_SERVER_TASK_STACK_DEPTH  , (void*)AF_INET, 5, NULL);
                // Check Remote Server
                // if(isCheckingRemoteServerIdle){
                //     xTaskCreate(check_remote_server_status, "http_test", 4096, NULL, 5, NULL);
                // }
                // xTaskCreate(pingMainTask, "mainTask", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
            
            }     
            CYP_LOG("Failed to stop dhcp client");      
            return;
        }

        esp_netif_ip_info_t temp_ip_info = (event)->ip_info;

        int temp_gateway_array[4] = {0};
        get_gateway_ip_info_array(temp_gateway_array , event->ip_info.gw.addr);

        int unused_ip = find_unused_ip(temp_gateway_array[0] , temp_gateway_array[1] , 
                                        temp_gateway_array[2] , temp_gateway_array[3]);

        if(unused_ip != -1){
            esp_netif_set_ip4_addr(&(temp_ip_info.ip) ,  
                                    temp_gateway_array[0] , temp_gateway_array[1] , 
                                    temp_gateway_array[2] , unused_ip);
            // esp_netif_set_ip4_addr(&(temp_ip_info.ip) , 172 , 16 , 225 , 13);      
            esp_err_t res = esp_netif_set_ip_info(event->esp_netif, &temp_ip_info);
            printf("STATIC IP RES: %X\r\n" , res);

            CYP_LOG("got ip:" IPSTR, IP2STR(&temp_ip_info.ip));
            update_wifi_ip_sta();                
        }

#else
        self->update_wifi_ip_sta();  
        self->isConnectedToWifi = true;
        if(!(self->all_sockets_init)){
            
            self->all_sockets_init = true;
            // Enable TCP Server
           // xTaskCreate(tcp_server_task, "tcp_server", TCP_SERVER_TASK_STACK_DEPTH  , (void*)AF_INET, 5, NULL);
           
        }

#endif        
        
        self->s_retry_num = 0;
        xEventGroupSetBits(self->s_wifi_event_group, WIFI_CONNECTED_BIT);
        
    }
}

void WifiStationMode::update_wifi_ip_sta(){

    int tempIpSlice = wifiInfoClass->get_device_ip_info_int();
    wifiInfoClass->update_wifi_mode_ip(tempIpSlice);

    // wifiInfoClass->ip4_len = 0;
    // if(wifiInfoClass->ip4 < 10){
    //     wifiInfoClass->ip4_len = 1;
    // }else if(wifiInfoClass->ip4 < 100){
    //     wifiInfoClass->ip4_len = 2;
    // }else if(wifiInfoClass->ip4 < 1000){
    //     wifiInfoClass->ip4_len = 3;
    // }

    // wire_con_alive_slave_size_len = 0;
    // if(wire_con_alive_slave_size < 10){
    //     wire_con_alive_slave_size_len = 1;
    // }else if(wire_con_alive_slave_size < 100){
    //     wire_con_alive_slave_size_len = 2;
    // }else if(wire_con_alive_slave_size < 1000){
    //     wire_con_alive_slave_size_len = 3;
    // }   
}

void WifiStationMode::wifi_init_sta()
{

    wifiInfoClass = new WifiInfoClass();

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
    //ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
    //ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id));
    //vEventGroupDelete(s_wifi_event_group);

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