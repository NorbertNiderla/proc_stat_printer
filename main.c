





int program(void* arg){
    
    

    return 0;
}

int main(void){
    memset(prev_processed_data, 0, 4 * sizeof(proc_data_total_t));
    thrd_t task_program;
    thrd_create(&task_program, program, NULL);
    thrd_join(task_program, NULL);
    return 0;
}