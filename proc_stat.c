#include "proc_stat.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <threads.h>
#include <time.h>
#include <stdbool.h>

proc_data_t* read_data_from_proc_stat(int* number_of_cpus){
    char buffer[128] = { 0 };
    FILE* file = fopen("/proc/stat", "r");
    if(file == NULL){
        return NULL;
    }

    proc_data_t* output = NULL;
    proc_data_t data = PROC_DATA_STATIC_INIT;
    *number_of_cpus = 0;
    int n = 0;
    bool all_cpus = true;

    while(1){
        char c = '\0';
        int buffer_index = 0;
        memset(buffer, 0, 128);
        while(c != '\n' && buffer_index < 128){
            c = getc(file);
            if(c == '\0'){
                fclose(file);
                *number_of_cpus = n;
                return output;
            }
            buffer[buffer_index++] = c;
        }

        if(all_cpus == true){
            all_cpus = false;
            continue;
        }

        if(sscanf(buffer, "cpu%*d %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
            &data.user, &data.nice, &data.system, &data.idle,
            &data.iowait, &data.irq, &data.soft_irq, &data.steal,
            &data.guest, &data.guest_nice) != 10){
            fclose(file);
            *number_of_cpus = n;
            return output;
        }

        n++;
        output = realloc(output, sizeof(proc_data_t) * n);
        memcpy(&output[n-1], &data, sizeof(proc_data_t));
    }

    fclose(file);
    return NULL;
}

void print_proc_data(proc_data_t* ptr){
    printf("%lu %lu %lu %lu %lu %lu %lu %lu %lu %lu\n", ptr->user, ptr->nice, ptr->system,
    ptr->idle, ptr->iowait, ptr->irq, ptr->soft_irq, ptr->steal, ptr->guest,
    ptr->guest_nice);
}

int calculate_proc_percentage(proc_data_total_t* prev, const proc_data_t* data){
    uint64_t real_idle = data->idle + data->iowait;
    uint64_t non_idle = data->user + data->nice + data->system +
            data->irq + data->soft_irq + data->steal;
    uint64_t total = real_idle + non_idle;
    int64_t totald = (int64_t)total - (int64_t)prev->total;
    int64_t idled = (int64_t)real_idle - (int64_t)prev->idle;

    prev->total = total;
    prev->idle = real_idle;

    return totald != 0 ? (totald - idled) * 100 / totald : 100;
}