// #include <string.h>
// #include <sys/param.h>

// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "esp_system.h"
// #include "esp_log.h"
// #include "esp_netif.h"
// #include "esp_event.h"
// #include "nvs.h"
// #include "nvs_flash.h"

// #include "lwip/err.h"
// #include "lwip/sockets.h"
// #include "lwip/sys.h"
// #include <lwip/netdb.h>

// #include "esp_wifi.h"
// #include "constants.h"
// #include "params.h"
// #include "Eeprom.h"
// #include "strLib.h"
// #include "WebSocket.h"
#include "stdbool.h"


struct account_struct {
  int  account_id;  
  char username[128];
  char password[8];
  int  is_admin;
  int  is_enable;
  int  sock;
  int  ip4; 
  char lockPassword[128];
};

extern struct account_struct account_struct_list[100];
extern int   account_list_size;

extern int  temp_sock           ;
extern int  tcp_timeout_counter ;
extern int  tcp_rec_data_counter;
extern bool is_tcp_timeout      ;
extern bool valid_data_received ;
extern char ASSIGN_WIRE_CON_ID[5];

void init_tcp_server();
void execute_tcp_send(char *data);
void send_assign_slave_id_response(int sock , char *response);
void tcp_send_to_clients(char *data , int len);
void process_tcp_data(char* rx_buffer , int sock);
void check_tcp_recv_timeout_task();
void remove_tcp_sock_from_list(int index);
void tcp_server_task(void *pvParameters);

