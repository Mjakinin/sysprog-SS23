#include "../lib/LCFSPR.h"
#include <stdio.h>

static queue_object* LCFSPR_queue;
//You can add more global variables here

process* LCFSPR_tick (process* running_process){
    
    if(running_process==NULL || running_process->time_left==0)
    {
        running_process = queue_poll(LCFSPR_queue);
    }

    if (running_process!=NULL && running_process->time_left > 0)
    {
        running_process->time_left--;
    }

    return running_process;
}

int LCFSPR_startup(){

    LCFSPR_queue=new_queue();

    if(LCFSPR_queue==NULL)
    {
        return 1;
    }

    return 0;
}

process* LCFSPR_new_arrival(process* arriving_process, process* running_process){

    if(arriving_process != NULL)
    {
        if(running_process != NULL)
        {
            queue_add(running_process, LCFSPR_queue);
        }

        running_process = arriving_process;
        
        return running_process;
    }
    else
    {
        return running_process;
    }

}

void LCFSPR_finish(){
    free_queue(LCFSPR_queue);
}


