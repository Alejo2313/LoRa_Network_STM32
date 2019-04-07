#include "circular.h"
#include <stdio.h>





void create_buffer(buffer_c* this, uint8_t size){


    this->data = (uint8_t*)malloc(size*sizeof(uint8_t));
    this->front = -1;
    this->rear = -1;
    this->size = size;
}


int full_buffer(buffer_c* this){
    if( (this->front == this->rear + 1) || (this->front == 0 && this->rear == this->size -1)) return 1;
    return 0;
}


int empty_buffer(buffer_c* this){
    if(this->front == -1)
        return 1;
    return 0;
}


void push_buffer(buffer_c* this, uint8_t pData)
{

    if(full(this))
        return;

    if(pData == 0){
        pData = 0;
    }

    else
    {
        if(this->front == -1) 
            this->front = 0;

        this->rear = (this->rear + 1) % this->size;
        *(this->data + this->rear) = pData;
    }
}

uint8_t pop_buffer(buffer_c* this)
{
    uint8_t element;

    if(empty(this)) {
        return NULL;
    } 

    else {
        element = *(this->data + this->front);

    if(element == 0){
        element = 0;
    }

        if (this->front == this->rear){
            this->front = -1;
            this->rear = -1;
        }
        else {
            this->front = (this->front + 1) % this->size;
            
        }
        return(element);
    }
}
void flush_buffer(buffer_c* this){
}