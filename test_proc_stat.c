#include <time.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include "proc_stat.h"

static proc_data_total_t prev_processed_data[4];

int main(void){
    int tries = 0;
    proc_data_t* data = NULL;
    int number_of_cpus = 0;

    while(tries++ < 5){
        data = read_data_from_proc_stat(&number_of_cpus);
        assert(data != NULL);
        assert(number_of_cpus > 0 && number_of_cpus < 10);
        printf("%d: ", number_of_cpus);
        int prc[4];

        for(int i = 0; i < number_of_cpus; i++){
            prc[i] = calculate_proc_percentage(&prev_processed_data[i], &data[i]);
            assert(prc[i] >=0 && prc[i] <= 100);
            printf("%d%% ", prc[i]); 
        }
        printf("\n");

        free(data);
        
        sleep(1);
    }
}