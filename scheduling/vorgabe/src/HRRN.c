#include "../lib/HRRN.h"

static queue_object* HRRN_queue;
//You can add more global variables and structs here

process* HRRN_tick (process* running_process){

    queue_object* current = HRRN_queue->next;

    while(current != NULL)
    {
        HRRN_process* hrrn_process = (HRRN_process*)current->object;
        hrrn_process->waiting_time++;
        current = current->next;
    }

    if(running_process ==NULL || running_process->time_left == 0)
    {
        if(HRRN_queue->next == NULL)
        {
            return NULL;
        }

        queue_object* prev = HRRN_queue;
        queue_object* highestrr_process = HRRN_queue->next;
        unsigned int highest_rr = 0;
        queue_object* current_process = HRRN_queue;

        while(current_process->next != NULL)
        {
            HRRN_process* hrrn_process = (HRRN_process*)current_process->next->object;

            hrrn_process->rr = (hrrn_process->waiting_time / ((HRRN_process*)current_process->next->object)->this_process->time_left) + 1;

            if(    hrrn_process->rr     >     highest_rr)
            {
                highest_rr = hrrn_process->rr;
                highestrr_process = current_process->next;
                prev = current_process;
            }

            current_process = current_process->next;
        }

        prev->next = highestrr_process->next;
        running_process = ((HRRN_process*)highestrr_process->object)->this_process;

        free(highestrr_process);
    }

    if(running_process!=NULL)
    {
        running_process->time_left--;
    }

    return running_process;
}

int HRRN_startup(){

    HRRN_queue=new_queue();

    if(HRRN_queue == NULL)
    {
        return 1;
    }

    return 0;
}

process* HRRN_new_arrival(process* arriving_process, process* running_process){
    if(arriving_process!=NULL){

        HRRN_process* tmp = (HRRN_process*)malloc(sizeof(HRRN_process));
        tmp->this_process = arriving_process;
        tmp->waiting_time = 0;
        tmp->rr = 0.0;

        queue_add(tmp, HRRN_queue);
    }

    return running_process;
}

void HRRN_finish(){
    free_queue(HRRN_queue);
    HRRN_queue = NULL;
}
