#include "../lib/SJN.h"
#include "limits.h"

static queue_object* SJN_queue;
//You can add more global variables here

process* SJN_tick (process* running_process){

    if(running_process == NULL || running_process->time_left==0)
    {
        if(SJN_queue->next == NULL)
        {
            return NULL;
        }

        queue_object* prev = SJN_queue;
        queue_object* shortest_process = SJN_queue->next;
        unsigned int shortest_time = INT_MAX;
        queue_object* current_process = SJN_queue;

        while(current_process->next != NULL)
        {
            if(    ((process*)current_process->next->object)->time_left      <     shortest_time)
            {
                shortest_time = ((process*)current_process->next->object)->time_left;
                shortest_process = current_process->next;
                prev = current_process;
            }

            current_process = current_process->next;
        }

        prev->next = shortest_process->next;
        running_process = shortest_process->object;

        free(shortest_process);
    }
    if(running_process != NULL){
        running_process->time_left--;
    }

    return running_process;

}

int SJN_startup(){

    SJN_queue=new_queue();

    if(SJN_queue == NULL)
    {
        return 1;
    }

    return 0;
}

process* SJN_new_arrival(process* arriving_process, process* running_process){

    if(arriving_process != NULL)
    {
        queue_add(arriving_process, SJN_queue);
    }

    return running_process;
}

void SJN_finish(){
    free_queue(SJN_queue);
}
