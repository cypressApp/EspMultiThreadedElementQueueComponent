#include <iostream>
#include <string>
#include <optional>
#include "CustomQueue.hpp"

extern "C"{
    #include "freertos/FreeRTOS.h"
    #include "freertos/semphr.h"  
    #include "esp_log.h"
    #include "freertos/task.h"
    #include <stdio.h>
    #include "lwip/inet.h"
    #include "lwip/netdb.h"
    #include "lwip/sockets.h"
    #include "esp_netif.h"
}

using namespace std;

/**
 * @brief tcp account class
 * 
 * this class is used to store all information of connected client
 *  
 */
class TcpAccount{

public:

    TcpAccount(){
        sock = -1;
    }

    int  sock;
    int  ip4; 

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

    int  tcp_rec_data_counter = 0;
    TcpAccount *tcpAccount;
    CustomQueue<int> *customQueue;
    /**
     * @brief object initialization
     * 
     * Some of the property of the object of class are initialized.
     * 
     */
    void initTcpServer();

    /**
     * @brief semaphore initialization
     * 
     * this semaphore is used syncronation wifi connection and tcp server socket.
     * 
     */
    static void initSemaphore();
    
    /**
     * @brief get unused socket
     * 
     * This function return an unused socket in the account list
     * 
     */
    optional<int> is_unused_tcp_socket_exist(int ip);
    
    /**
     * @brief process received data from client
     * 
     * The received data is process to extract commands (e.g. push, pop, size)
     * 
     */
    void process_tcp_data(char* rx_buffer , int accountIndex , int sock);
    
    /**
     * @brief tcp receive data handler
     * 
     * This function wait for data from client
     * 
     */
    void runReceivingTcpData(int accountIndex , int sock);
    
    /**
     * @brief receive data entry
     * 
     * this function run receive data handler in another task
     * 
     */
    static void receiveTcpEntry(void *pvParamters);

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

/**
 * @brief task params struct
 * 
 * This struct is used to pass the required params to run the freeRTOS task
 * 
 */
typedef struct {
    TcpServerClass *tcpServer;
    int sock;
    int account_index;
} ReceiveTcpStruct;