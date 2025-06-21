#pragma once

#include <iostream>
#include <future>
#include <chrono>
#include <thread>
#include <vector>
#include <optional>
#include "CustomQueue.hpp"

using namespace std;

template<typename T>
QueueElement<T>::QueueElement(T data){
    this->data = data;
    next = nullptr;
}

template<typename T>
CustomQueue<T>::CustomQueue(size_t maxSize){
    this->maxSize = maxSize;
    size = 0;
    head = nullptr;
    tail = nullptr;
    counter = 0;
}

template<typename T>
bool CustomQueue<T>::isEmpty(){
    return size == 0;
}

template<typename T>
bool CustomQueue<T>::isFull(){
    return size == maxSize;
}

template<typename T>
void CustomQueue<T>::execPush(T data){

    if(isEmpty()){
        head = new QueueElement(data);
        tail = head;
        size++;
        return;
    }
    
    QueueElement<T> *temp = new QueueElement(data);
    tail->next = temp;
    tail = temp;
    
    size++;

}

template<typename T>
int CustomQueue<T>::push(T data , int timeout, bool showWarning){

    auto start = chrono::steady_clock::now();

    counter++;

    if(timeout > 0){
        if (!push_pop_tmtx.try_lock_for(chrono::milliseconds(timeout))) {
            return CustomQueue<T>::OP_FAIL;
        }
    }else{
        push_pop_tmtx.lock();
    }

    bool tempIsEmpty = isEmpty();

    
    while(isFull()){
        if(showWarning) {
            cout << "\033[33m" << "Queue overflow!" << "\033[0m" << endl;
            showWarning = false;
        }
        push_pop_tmtx.unlock();
        if(timeout > 0){
            while (isFull()) {
                if (chrono::steady_clock::now() - start >= chrono::milliseconds(timeout)) {
                    if(tempIsEmpty){
                        semPop.release();
                    }
                    return CustomQueue<T>::OP_FAIL;
                }
            }
        }else{
            semPush.acquire();   
        }
        push_pop_tmtx.lock();
    }

    if(timeout == 0){
        execPush(data);
    }else{
        if (chrono::steady_clock::now() - start <= chrono::milliseconds(timeout)) {
            execPush(data);
        }else{
            if(tempIsEmpty){
                semPop.release();
            }
    
            push_pop_tmtx.unlock();
            return CustomQueue<T>::OP_FAIL;
        } 
    }

    if(tempIsEmpty){
        semPop.release();
    }
    
    push_pop_tmtx.unlock();

    return CustomQueue::OP_SUCCESS;

}

template<typename T>
T CustomQueue<T>::execPop(){

    int data = head->data;
    
    QueueElement<T> *temp = head;
    
    if(head == tail){
        head = nullptr;
        tail = nullptr;
    }else{
        head = temp->next;
    }
    
    delete(temp);
    
    size--;
    
    return data;

}

template<typename T>
optional<T> CustomQueue<T>::pop(int timeout, bool showWarning){

    auto start = chrono::steady_clock::now();

    counter++;

    if(timeout > 0){
        if (!push_pop_tmtx.try_lock_for(chrono::milliseconds(timeout))) {
            return nullopt;
        }
    }else{
        push_pop_tmtx.lock();
    }

    
    optional<T> result = nullopt;    
    
    bool tempIsFull = isFull();
    
    while(isEmpty()){
        if(showWarning) {
            cout << "\033[33m" << "Queue underflow" << "\033[0m" << endl;
            showWarning = false;
        }
        push_pop_tmtx.unlock();
        if(timeout > 0){
            while (isEmpty()) {
                if (chrono::steady_clock::now() - start >= chrono::milliseconds(timeout)) {
                    if(tempIsFull){
                        semPush.release();
                    }
                    return result;
                }
            }
        }else{
            semPop.acquire();
        }
        push_pop_tmtx.lock();
    }

    if(timeout == 0){
        result = execPop();
    }else{
        if (chrono::steady_clock::now() - start < chrono::milliseconds(timeout)) {
            result = execPop();
        }         
    }

    if(tempIsFull){
        semPush.release();
    }

    push_pop_tmtx.unlock();

    return result;
}

template<typename T>
size_t CustomQueue<T>::getSize(){
    return size;
}

template<typename T>
int CustomQueue<T>::getCounter(){
    return counter;
}

template<typename T>
size_t CustomQueue<T>::getMaxQueueCapacity(){
    return maxSize;
}