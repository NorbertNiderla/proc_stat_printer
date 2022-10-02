#include <threads.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include "proc_stat.h"
#include "ringbuffer.h"

static ringbuffer_t analyzer_buffer;
static ringbuffer_t printer_buffer;
static cnd_t analyzer_feed_cnd;
static mtx_t analyzer_feed_mtx;
static cnd_t printer_feed_cnd;
static mtx_t printer_feed_mtx;

static const struct timespec cond_waittime = {.tv_sec = 1};
volatile sig_atomic_t done = 0;
 
void term(int signum){
    done = signum;
    done = 1;
}

int reader(void* arg){
    const struct timespec delay = {.tv_sec = 1};
    proc_data_t* data = NULL;
    int n = *((int*)arg);
    while(!done){
        data = read_data_from_proc_stat(&n);
        if(data != NULL && n > 0){
            ringbuffer_append(analyzer_buffer, (void*)data);
            mtx_lock(&analyzer_feed_mtx);
            cnd_signal(&analyzer_feed_cnd);
            mtx_unlock(&analyzer_feed_mtx);
        }

        thrd_sleep(&delay, NULL);
    }

    return 0;
}

int analyzer(void* arg){
    proc_data_t* data = NULL;
    proc_data_total_t* prev = NULL;
    int* prc = NULL;
    bool prev_initialized = false;
    int number_of_cpus = *((int*)arg);
    while(!done){
        data = (proc_data_t*)ringbuffer_pop(analyzer_buffer);
        if(data == NULL){
            mtx_lock(&analyzer_feed_mtx);
            cnd_timedwait(&analyzer_feed_cnd, &analyzer_feed_mtx, &cond_waittime);
            mtx_unlock(&analyzer_feed_mtx);
            continue;
        }

        if(!prev_initialized){
            prev_initialized = true;
            prev = malloc(number_of_cpus * sizeof(proc_data_total_t));
        }

        prc = malloc(number_of_cpus * sizeof(int));
        for(int i = 0; i < number_of_cpus; i++){
            prc[i] = calculate_proc_percentage(&prev[i], &data[i]);
        }

        free(data);

        ringbuffer_append(printer_buffer, (void*)prc);
        mtx_lock(&printer_feed_mtx);
        cnd_signal(&printer_feed_cnd);
        mtx_unlock(&printer_feed_mtx);
    }

    return 0;
}

int printer(void* arg){
    int* prc = NULL;
    const struct timespec delay = {.tv_sec = 1};
    int number_of_cpus = *((int*)arg);
    while(!done){
        prc = ringbuffer_pop(printer_buffer);
        if(prc == NULL){
            mtx_lock(&printer_feed_mtx);
            cnd_timedwait(&printer_feed_cnd, &printer_feed_mtx, &cond_waittime);
            mtx_unlock(&printer_feed_mtx);
            continue;
        }

        for(int i = 0; i < number_of_cpus; i++){
            printf("%d%% ", prc[i]);
        }
        printf("\n");
        free(prc);
        thrd_sleep(&delay, NULL);
    }

    return 0;
}

int main(void){
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = term;
    sigaction(SIGTERM, &action, NULL);

    /* is it safe assumption that number of cores won't change during
    * life of program? */
    int number_of_cpus = 0;
    proc_data_t* temp = read_data_from_proc_stat(&number_of_cpus);
    free(temp);

    analyzer_buffer = ringbuffer_create();
    printer_buffer = ringbuffer_create();
    cnd_init(&analyzer_feed_cnd);
    cnd_init(&printer_feed_cnd);
    mtx_init(&analyzer_feed_mtx, mtx_plain);
    mtx_init(&printer_feed_mtx, mtx_plain);

    thrd_t reader_thread, analyzer_thread, printer_thread;
    
    thrd_create(&reader_thread, reader, (void*)&number_of_cpus);
    thrd_create(&analyzer_thread, analyzer, (void*)&number_of_cpus);
    thrd_create(&printer_thread, printer, (void*)&number_of_cpus);
    
    thrd_join(reader_thread, NULL);
    thrd_join(analyzer_thread, NULL);
    thrd_join(printer_thread, NULL);

    mtx_destroy(&analyzer_feed_mtx);
    mtx_destroy(&printer_feed_mtx);
    cnd_destroy(&analyzer_feed_cnd);
    cnd_destroy(&printer_feed_cnd);
    ringbuffer_destroy(analyzer_buffer);
    ringbuffer_destroy(printer_buffer);
    return 0;
}