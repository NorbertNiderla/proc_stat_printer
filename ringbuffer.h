/**
 * @file ringbuffer.h
 * @author Norbert Niderla (norbert.niderla@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-10-02
 * 
 * @copyright Copyright (c) 2022
 * 
 * Thread-safe ringbuffer for storing void pointers.
 */

#ifndef PROC_STAT_PRINTER_RINGBUFFER_H
#define PROC_STAT_PRINTER_RINGBUFFER_H

#define RINGBUFFER_SIZE (4)

typedef void* ringbuffer_t;

ringbuffer_t ringbuffer_create(void);

/**
 * @brief 
 * 
 * @param buffer 
 * @param ptr 
 * @retval 0 on success
 * @retval -1 on error(ie full buffer)
 */
int ringbuffer_append(ringbuffer_t buffer, void* ptr);

void* ringbuffer_pop(ringbuffer_t buffer);
void ringbuffer_destroy(ringbuffer_t buffer);

#endif