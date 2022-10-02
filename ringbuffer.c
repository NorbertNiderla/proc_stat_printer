#include "ringbuffer.h"
#include <stdlib.h>
#include <threads.h>
#include <stdbool.h>
#include <string.h>

typedef struct ringbuffer_struct{
    int start;
    int stop;
    bool empty;
    void* buffer[RINGBUFFER_SIZE];
    mtx_t mtx;
} ringbuffer_struct_t;

ringbuffer_t ringbuffer_create(void){
    ringbuffer_struct_t* this = malloc(sizeof(ringbuffer_struct_t));
    if(this == NULL){
        return NULL;
    }
    this->start = 0;
    this->stop = 0;
    this->empty = true;
    memset(this->buffer, 0, RINGBUFFER_SIZE*sizeof(void*));
    if(mtx_init(&this->mtx, mtx_plain) != thrd_success){
        free(this);
        return NULL;
    }

    return this;
}


int ringbuffer_append(ringbuffer_t buffer, void* ptr){
    ringbuffer_struct_t* this = (ringbuffer_struct_t*)buffer;
    
    mtx_lock(&this->mtx);
    if(!this->empty){
        if(this->start == this->stop){
            mtx_unlock(&this->mtx);
            return -1;
        }
    }

    this->empty = false;
    this->buffer[this->stop++] = ptr;
    this->stop %= RINGBUFFER_SIZE;
    mtx_unlock(&this->mtx);
    return 0;
}

void* ringbuffer_pop(ringbuffer_t buffer){
    ringbuffer_struct_t* this = (ringbuffer_struct_t*)buffer;
    
    mtx_lock(&this->mtx);
    if(this->empty){
        mtx_unlock(&this->mtx);
        return NULL;
    }

    void* ptr = this->buffer[this->start++];
    this->start %= RINGBUFFER_SIZE;
    if(this->start == this->stop){
        this->empty = true;
    }
    mtx_unlock(&this->mtx);
    return ptr;
}

void ringbuffer_destroy(ringbuffer_t buffer){
    ringbuffer_struct_t* this = (ringbuffer_struct_t*)buffer;
    mtx_destroy(&this->mtx);
    void* data = ringbuffer_pop(buffer);
    while(data != NULL){
        free(data);
        data = ringbuffer_pop(buffer);
    }
    free(this);
}

