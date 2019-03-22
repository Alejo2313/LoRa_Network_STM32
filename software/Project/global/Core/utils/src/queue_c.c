
#include "queue_c.h"
#include "stdio.h"





void create_queue(queue_c* this, uint8_t size){

    this->data = (void**)pvPortMalloc(size*sizeof(void*));
    this->front = -1;
    this->rear = -1;
    this->size = size;
}


int full(queue_c* this){
    if( (this->front == this->rear + 1) || (this->front == 0 && this->rear == this->size -1)) return 1;
    return 0;
}
int empty(queue_c* this){
    if(this->front == -1)
        return 1;
    return 0;
}


void push(queue_c* this, void* pData)
{

    if(full(this))
        return;

    else
    {
        if(this->front == -1) 
            this->front = 0;

        this->rear = (this->rear + 1) % this->size;
        *(this->data + this->rear) = pData;
    }
}

void* pop(queue_c* this)
{
    void* element;
    if(empty(this)) {
        return NULL;
    } 

    else {
        element = *(this->data + this->front);
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
void flush(queue_c* this){
}