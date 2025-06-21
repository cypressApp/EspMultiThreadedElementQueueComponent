#include <iostream>
#include <string>
#include <optional>
#include "CustomQueue.hpp"

extern "C"{
    #include "freertos/FreeRTOS.h"
    #include "freertos/semphr.h"  
    #include "esp_log.h"

    #include "freertos/task.h"
    #include "freertos/event_groups.h"
    #include "esp_system.h"
    #include "driver/uart.h"
    #include "string.h"
    #include <string.h>
    #include <stdio.h>
    #include <math.h>
    #include "driver/gpio.h"
    #include "esp_mac.h"
    #include "nvs_flash.h"
    #include "esp_partition.h"
    #include "esp_ota_ops.h"
    #include <sys/param.h>
    #include "esp_netif.h"
    #include "esp_event.h"
    #include "driver/touch_pad.h"
    #include <stdlib.h>
    #include "soc/soc_caps.h"
    #include "esp_adc/adc_oneshot.h"
    #include "esp_adc/adc_cali.h"
    #include "esp_adc/adc_cali_scheme.h"
    #include "esp_wifi.h"
    #include "esp_err.h"
    #include "esp_check.h"
    #include "lwip/err.h"
    #include "lwip/sys.h"

    #include "sdkconfig.h"
    #include "lwip/inet.h"
    #include "lwip/netdb.h"
    #include "lwip/sockets.h"
    #include "esp_console.h"
    #include "argtable3/argtable3.h"
    #include "ping/ping_sock.h"

    #include "lwip/sockets.h"
    #include "esp_netif.h"
}

using namespace std;

class TcpAccount{

public:

    TcpAccount(){
        sock = -1;
    }

    int  account_id;  
    char username[128];
    char password[8];
    int  is_admin;
    int  is_enable;
    int  sock;
    int  ip4; 
    char lockPassword[128];

};

class TcpServerClass {

public:
    
    static SemaphoreHandle_t tcpServerSemaphore;
    static const string TAG;
    static const int MAX_TCP_ACCOUNT_SIZE;
    static const int CLOSE_TCP_SOCKET_DELAY;
    static const int TCP_RECEIVE_DATA_LENGTH;
    static const int TCP_PORT;
    static const string TCP_RECEIVE_DATA_SUFFIX;
    static const int TCP_RECEIVE_DATA_SUFFIX_LENGTH ;

    int  tcp_timeout_counter  = 0    ;
    int  tcp_rec_data_counter = 0    ;
    bool is_tcp_timeout       = false;
    bool valid_data_received  = false;
    // struct account_struct account_struct_list[MAX_TCP_ACCOUNT_SIZE];
    TcpAccount *tcpAccount;
    int account_list_size = 0;
    CustomQueue<int> *customQueue;

    void init_tcp_server();
    void execute_tcp_send(char *data);
    void send_assign_slave_id_response(int sock , char *response);
    void tcp_send_to_clients(char *data , int len);
    void process_tcp_data(char* rx_buffer , int accountIndex , int sock);
    void remove_tcp_sock_from_list(int index);
    void tcp_server_task(void *pvParameters);
    optional<int> is_unused_tcp_socket_exist(int ip);
    void run_receiving_tcp_data(int accountIndex , int sock);
    void check_tcp_recv_timeout_task(void *pvParameters);

    static void init();

    static void receive_tcp_entry(void *pvParamters);

    static void task_entry(void *pvParamters);
    void run();
    void start();

};

typedef struct {
    TcpServerClass *tcpServer;
    int sock;
    int account_index;
} ReceiveTcpStruct;