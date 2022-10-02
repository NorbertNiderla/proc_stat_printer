#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "ringbuffer.h"


int main(void){
    ringbuffer_t buffer = ringbuffer_create();

    void* data[] = {(void*)1, (void*)2, (void*)3, (void*)4};

    /* test full buffer */
    for(int i = 0; i < 4; i++){
        ringbuffer_append(buffer, data[i]);
    }
    int ret = ringbuffer_append(buffer, data[0]);
    assert(ret == -1);

    /* test values from buffer */
    for(int i = 0; i < 4; i++){
        void* val = ringbuffer_pop(buffer);
        assert(val == data[i]);
    }

    /* test empty buffer */
    void* val = ringbuffer_pop(buffer);
    assert(val == NULL);

    printf("test_ringbuffer success\n");
    return 0;
}