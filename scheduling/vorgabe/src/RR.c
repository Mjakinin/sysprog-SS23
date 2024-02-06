#include "../lib/RR.h"
#include <stdio.h>

static queue_object* RR_queue;
//You can add more global variables
int quantum_reset;
int quantum_remaining;

process* RR_tick (process* running_process){

    if(running_process != NULL)
    {
        running_process->time_left--;
        quantum_remaining--;

        if(quantum_remaining == 0  &&  running_process->time_left  >  0)
        {
            queue_add(running_process, RR_queue); 
            running_process=NULL; 
        }
    }

    if(running_process == NULL  ||  running_process->time_left == 0)
    {
        running_process=queue_poll(RR_queue);
        quantum_remaining = quantum_reset;
    }

    return running_process;
}

int RR_startup(int quantum){

    RR_queue=new_queue();

    if(RR_queue==NULL)
    {
        return 1;
    }

    quantum_reset = quantum;
    quantum_remaining = quantum;

    return 0;
}


process* RR_new_arrival(process* arriving_process, process* running_process){

    if(arriving_process!=NULL)
    {
        queue_add(arriving_process, RR_queue);
    }

    return running_process;
}


void RR_finish(){
    free_queue(RR_queue);
}
