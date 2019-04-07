
#ifndef __CIRCULAR_H__
#define __CIRCULAR_H__

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>


typedef struct buffer_c
{
    uint8_t* data;
    int front, rear;
    uint8_t size;
}buffer_c;


void create_buffer(buffer_c*, uint8_t size);

void push_buffer(buffer_c* this, uint8_t pData);
uint8_t pop_buffer(buffer_c* this);
void flush_buffer(buffer_c* this);
int  empty_buffer(buffer_c* this);
int full_buffer(buffer_c* this);




#endif