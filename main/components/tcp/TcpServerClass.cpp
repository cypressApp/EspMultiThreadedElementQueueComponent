#include "TcpServerClass.hpp"
#include <cstdio>
#include <cstring>

using namespace std;

const string TcpServerClass::TAG = "TcpServerClass";
SemaphoreHandle_t TcpServerClass::tcpServerSemaphore = nullptr;

#define CYP_LOGI(fmt, ...) ESP_LOGI((TcpServerClass::TAG).c_str(), fmt, ##__VA_ARGS__)
#define CYP_LOGE(fmt, ...) ESP_LOGE((TcpServerClass::TAG).c_str(), fmt, ##__VA_ARGS__)

const int TcpServerClass::MAX_TCP_ACCOUNT_SIZE = 100;
const int TcpServerClass::CLOSE_TCP_SOCKET_DELAY = 100;
const int TcpServerClass::TCP_RECEIVE_DATA_LENGTH = 8192;
const int TcpServerClass::TCP_PORT = 1234;
const string TcpServerClass::TCP_RECEIVE_DATA_SUFFIX = "\n";
const int TcpServerClass::TCP_RECEIVE_DATA_SUFFIX_LENGTH = 1;

void TcpServerClass::initSemaphore() {
    tcpServerSemaphore = xSemaphoreCreateBinary();
    if (tcpServerSemaphore == nullptr) {
        CYP_LOGE("Failed to create semaphore");
    } else {
        CYP_LOGI("Semaphore created");
    }
}

void TcpServerClass::task_entry(void *pvParamters){
    TcpServerClass *self = static_cast<TcpServerClass*>(pvParamters);
    self->run();
}

void TcpServerClass::initTcpServer(){
    tcpAccount = new TcpAccount[MAX_TCP_ACCOUNT_SIZE];
    customQueue = new CustomQueue<int>[MAX_TCP_ACCOUNT_SIZE]; 
}

void TcpServerClass::process_tcp_data(char* rx_value , int accountIndex , int sock){

    int pushValue = -1;

    if(sscanf(rx_value, "push(%d)" , &pushValue) == 1){
        if(customQueue[accountIndex].push(pushValue , 10 , true) == CustomQueue<int>::OP_SUCCESS){
            string response = "push: " + to_string(pushValue) + "\n";
            send(tcpAccount[accountIndex].sock, (response.c_str()), response.length(), 0);
        }
        
    }else if(string(rx_value) == "pop()"){
        optional<int> popValue = customQueue[accountIndex].pop(10 , true);
        if(popValue.has_value()){
            string response = "pop: " + to_string(popValue.value()) + "\n";
            send(tcpAccount[accountIndex].sock, (response.c_str()), response.length(), 0);
        }
        
    }else if(string(rx_value) == "size"){
        string response = "size: " + to_string(customQueue[accountIndex].getSize()) + "\n";
        send(tcpAccount[accountIndex].sock, (response.c_str()), response.length(), 0);
    }

}

void TcpServerClass::runReceivingTcpData(int accountIndex , int sock){

    char temp_rx_buffer[5];
    char rx_buffer[TcpServerClass::TCP_RECEIVE_DATA_LENGTH];

    vTaskDelay(2 / portTICK_PERIOD_MS);
    tcp_rec_data_counter = 0;

    while (1) {

#ifdef CONSTANT_TCP_RECEIVE_LEN        
        int len = recv(sock, rx_buffer, TcpServerClass::TCP_RECEIVE_DATA_LENGTH , 0);
#else        
        int len = recv(sock, temp_rx_buffer, 1 , 0);
        tcp_rec_data_counter ++;
#endif        
        // Error occured during receiving
        if (len < 0) {
            CYP_LOGE("recv failed: errno %d", errno);
            break;
        }
        // Connection closed
        else if (len == 0) {
            CYP_LOGI("Connection closed");        
            break;
        }
        // Data received
        else {

#ifdef CONSTANT_TCP_RECEIVE_LEN        
            char temp_rx_buffer[TcpServerClass::TCP_RECEIVE_DATA_LENGTH + 1] = {0};
            for(int i = 0 ; i < TcpServerClass::TCP_RECEIVE_DATA_LENGTH ; i++){
                temp_rx_buffer[i] = rx_buffer[i];
            }
            temp_rx_buffer[TcpServerClass::TCP_RECEIVE_DATA_LENGTH] = 0;
            process_tcp_data(temp_rx_buffer , sock);
#else        
            rx_buffer[tcp_rec_data_counter - 1] = temp_rx_buffer[0];
            if(tcp_rec_data_counter >= TcpServerClass::TCP_RECEIVE_DATA_LENGTH){
                tcp_rec_data_counter = 0;
            }else if(tcp_rec_data_counter >= TcpServerClass::TCP_RECEIVE_DATA_SUFFIX_LENGTH ){
                    if(memcmp( (rx_buffer + tcp_rec_data_counter - TcpServerClass::TCP_RECEIVE_DATA_SUFFIX_LENGTH) , (TcpServerClass::TCP_RECEIVE_DATA_SUFFIX).c_str() , TcpServerClass::TCP_RECEIVE_DATA_SUFFIX_LENGTH ) == 0 ){
                        
                        rx_buffer[tcp_rec_data_counter - TcpServerClass::TCP_RECEIVE_DATA_SUFFIX_LENGTH] = 0; 
                        tcp_rec_data_counter = 0;
                        CYP_LOGI("%s", rx_buffer);

                        process_tcp_data(rx_buffer , accountIndex , sock);
                        vTaskDelay(1 / portTICK_PERIOD_MS);

                    }
            }

#endif  


        }
        
    }

    CYP_LOGE("Shutting down socket and restarting...");
    
    shutdown(tcpAccount[accountIndex].sock, 0);
    close(tcpAccount[accountIndex].sock);
    tcpAccount[accountIndex].sock = -1;

    vTaskDelete(NULL);
        
}

optional<int> TcpServerClass::is_unused_tcp_socket_exist(int ip){

    for(int i = 0 ; i < TcpServerClass::MAX_TCP_ACCOUNT_SIZE ; i++){
        if(tcpAccount[i].sock == -1){
            return i;
        }
    }    

    return nullopt;
}

void TcpServerClass::run(){
    
    CYP_LOGI("run tcp Socket");

    char addr_str[128];
    int addr_family = AF_INET;
    int ip_protocol = 0;
    struct sockaddr_storage dest_addr;
    int listen_sock;

    while(1){

        if (addr_family == AF_INET) {
            struct sockaddr_in *dest_addr_ip4 = (struct sockaddr_in *)&dest_addr;
            dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);
            dest_addr_ip4->sin_family = AF_INET;
            dest_addr_ip4->sin_port = htons(TcpServerClass::TCP_PORT);
            ip_protocol = IPPROTO_IP;
        }

        listen_sock = socket(addr_family, SOCK_STREAM, ip_protocol);
        if (listen_sock < 0) {
            CYP_LOGE("Unable to create socket: errno %d", errno);                     
            vTaskDelete(NULL);
            return;
        }

        int opt = 1;
        setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        CYP_LOGI("Socket created");
        int err = bind(listen_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));

        if (err != 0) {            
            CYP_LOGE("Socket unable to bind: errno %d", errno);
            CYP_LOGE("IPPROTO: %d", addr_family);           
        }      
        CYP_LOGI("Socket bound, port %d", TcpServerClass::TCP_PORT);

        err = listen(listen_sock, 10);

        if (err != 0) {
            CYP_LOGE("Error occurred during listen: errno %d", errno);
        }

        while (1) {
            vTaskDelay(10 / portTICK_PERIOD_MS);

            CYP_LOGI("Socket listening");
            
            struct sockaddr_storage source_addr; 
            socklen_t addr_len = sizeof(source_addr);
            int sock = accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);
            if (sock < 0) {
                CYP_LOGE("Unable to accept connection: errno %d", errno);
                break;
            }

            if (source_addr.ss_family == PF_INET) {
                inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, addr_str, sizeof(addr_str) - 1);
            }
            
            int ipArray[4];
            if(sscanf(addr_str, "%d.%d.%d.%d", &ipArray[0], &ipArray[1], &ipArray[2], &ipArray[3]) != 4){
                continue;
            }
            CYP_LOGI("Socket accepted ip address: %d", ipArray[3]);

            if(ipArray[3] < 255){
                
                optional<int> temp_tcp_sock_index = is_unused_tcp_socket_exist(ipArray[3]);

                if(temp_tcp_sock_index.has_value()){
                    int tempIndex = temp_tcp_sock_index.value();
                    close(tcpAccount[tempIndex].sock);
                    vTaskDelay(TcpServerClass::CLOSE_TCP_SOCKET_DELAY / portTICK_PERIOD_MS);
                    tcpAccount[tempIndex].sock = sock;
                    tcpAccount[tempIndex].ip4 = ipArray[3];
                    ReceiveTcpStruct receiveTcpStruct;
                    receiveTcpStruct.tcpServer = this;
                    receiveTcpStruct.sock = sock;
                    receiveTcpStruct.account_index = tempIndex;
                    xTaskCreate(TcpServerClass::receiveTcpEntry, "receiving_tcp_data", 1024, &receiveTcpStruct , 5, NULL); 

                    CYP_LOGI("Socket Exist");                  
                }else{
                    close(sock);
                } 

            }

        }
        
    }

    vTaskDelay(1000 / portTICK_PERIOD_MS);
    vTaskDelete(NULL);
}

void TcpServerClass::start(){

    TcpServerClass::initSemaphore();
    xSemaphoreTake(tcpServerSemaphore , portMAX_DELAY);

    initTcpServer();

    xTaskCreate(TcpServerClass::task_entry, "tcp_server", 20480  , this, 1, NULL);

}

void TcpServerClass::receiveTcpEntry(void *pvParamters){
    ReceiveTcpStruct *self = static_cast<ReceiveTcpStruct*>(pvParamters);
    self->tcpServer->runReceivingTcpData(self->account_index , self->sock);
}

