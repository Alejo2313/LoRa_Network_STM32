#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>


#define queu_init(typeof, size)

typedef struct queue_c
{
    void** data;
    int front, rear;
    uint8_t size;
}queue_c;


void create_queue(queue_c*, uint8_t size);

void push(queue_c* this, void* pData);
void* pop(queue_c* this);
void flush(queue_c* this);
int  empty(queue_c* this);
int full(queue_c* this);