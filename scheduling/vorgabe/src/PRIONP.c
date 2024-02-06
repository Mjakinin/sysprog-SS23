#include "../lib/PRIONP.h"
#include <stdio.h>
#include <stdbool.h>

static queue_object* PRIONP_queue;
//You can add more global variables here

process* PRIONP_tick (process* running_process){

    if(running_process == NULL || running_process->time_left == 0)
    {
        if(PRIONP_queue->next == NULL)
        {
            return NULL;
        }

        queue_object* prev = PRIONP_queue;
        queue_object* highest_process = PRIONP_queue->next;
        unsigned int highest_prio = 0;
        queue_object* current_process = PRIONP_queue;

        while(current_process->next != NULL)
        {
            if(    ((process*)current_process->next->object)->priority      >     highest_prio)
            {
                highest_prio = ((process*)current_process->next->object)->priority;
                highest_process = current_process->next;
                prev = current_process;
            }

            current_process = current_process->next;
        }

        prev->next = highest_process->next;
        running_process = highest_process->object;

        free(highest_process);
    }

    if(running_process!=NULL){
        running_process->time_left--;
    }

    return running_process;
}

int PRIONP_startup(){

    PRIONP_queue = new_queue();

    if(PRIONP_queue == NULL)
    {
        return 1;
    }

    return 0;
}

process* PRIONP_new_arrival(process* arriving_process, process* running_process){

    if(arriving_process != NULL)
    {
        queue_add(arriving_process, PRIONP_queue);
    }

    return running_process;
}

void PRIONP_finish(){
  free_queue(PRIONP_queue);
}