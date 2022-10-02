#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <threads.h>
#include <time.h>

typedef struct proc_data {
    uint64_t user;
    uint64_t nice;
    uint64_t system;
    uint64_t idle;
    uint64_t iowait;
    uint64_t irq;
    uint64_t soft_irq;
    uint64_t steal;
    uint64_t guest;
    uint64_t guest_nice;
} proc_data_t;

typedef struct proc_data_total{
    uint64_t total;
    uint64_t idle;
}proc_data_total_t;

int read_data_from_proc_stat(proc_data_t* ptr, int entries){
    FILE* file;
    char buffer[100];
    file = fopen("/proc/stat", "r");

    if(file == NULL){
        return -1;
    }

    for(int i = 0; i < entries; i++){
        char c = '\0';
        int buffer_index = 0;
        memset(buffer, 0, 100);
        while(c != '\n' && buffer_index < 100){
            c = getc(file);
            if(c == '\0'){
                fclose(file);
                return -3;
            }
            buffer[buffer_index++] = c;
        }

        if(sscanf(buffer, "%*s %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
            &ptr[i].user, &ptr[i].nice, &ptr[i].system, &ptr[i].idle,
            &ptr[i].iowait, &ptr[i].irq, &ptr[i].soft_irq, &ptr[i].steal,
            &ptr[i].guest, &ptr[i].guest_nice) != 10){
            fclose(file);
            return -2;
        }
    }

    fclose(file);
    return 0;
}

void print_proc_data(proc_data_t* ptr){
    printf("%lu %lu %lu %lu %lu %lu %lu %lu %lu %lu\n", ptr->user, ptr->nice, ptr->system,
    ptr->idle, ptr->iowait, ptr->irq, ptr->soft_irq, ptr->steal, ptr->guest,
    ptr->guest_nice);
}

/* new data will be written into prev */
/* proc percentage in percents will be returned */
int calculate_proc_percentage(proc_data_total_t* prev, const proc_data_t* data){
    uint64_t real_idle = data->idle + data->iowait;
    uint64_t non_idle = data->user + data->nice + data->system +
            data->irq + data->soft_irq + data->steal;
    uint64_t total = real_idle + non_idle;
    int64_t totald = (int64_t)total - (int64_t)prev->total;
    int64_t idled = (int64_t)real_idle - (int64_t)prev->idle;

    prev->total = total;
    prev->idle = real_idle;

    return (totald - idled) * 100 / totald;
}

static proc_data_t processed_data[5];
static proc_data_total_t prev_processed_data[4];

int program(void* arg){
    
    const struct timespec delay = {.tv_sec =  1};
        
    while(1){
        int ret = read_data_from_proc_stat(processed_data, 5);
        if(ret != 0){
            return -1;
        }
        
        int prc[4];
        for(int i = 0; i< 4; i++){
            prc[i] = calculate_proc_percentage(&prev_processed_data[i], &processed_data[i+1]);
        }
        printf("%d%% %d%% %d%% %d%%\n", prc[0], prc[1], prc[2], prc[3]); 
        
        thrd_sleep(&delay, NULL);
    }

    return 0;
}

int main(void){
    memset(prev_processed_data, 0, 4 * sizeof(proc_data_total_t));
    thrd_t task_program;
    thrd_create(&task_program, program, NULL);
    thrd_join(task_program, NULL);
    return 0;
}