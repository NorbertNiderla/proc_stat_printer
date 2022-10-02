/**
 * @file proc_stat.h
 * @author Norbert Niderla
 * @brief 
 * @version 0.1
 * @date 2022-10-02
 * 
 * @copyright Copyright (c) 2022
 * 
 * Functions and data structures for operating on data stored
 * in /proc/stat.
 */

#ifndef PROC_STAT_PRINTER_PROC_STAT_H
#define PROC_STAT_PRINTER_PROC_STAT_H

#include <stdint.h>

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

/**
 * @brief Reads data from /proc/stat and returns cpu usage data
 * in data structures.
 * 
 * @param[out] number_of_cpus 
 * @return proc_data_t* 
 */
proc_data_t* read_data_from_proc_stat(int* number_of_cpus);

void print_proc_data(proc_data_t* ptr);

/**
 * @brief New data will be written into prev proc percentage in
 * percents will be returned
 * 
 * @param prev 
 * @param data 
 * @return int 
 */
int calculate_proc_percentage(proc_data_total_t* prev, const proc_data_t* data);

#endif