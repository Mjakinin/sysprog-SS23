#include "../lib/MLF.h"
#include <stdio.h>

static queue_object** MLF_queues;
//You can add more global variables here
int quantums[4] = {1,2,6,-1};
int quantum_current = 0;
int quantum_remaining_mlf = 1;

process* MLF_tick(process* running_process) {

    if(quantum_remaining_mlf == 0 || running_process == NULL || running_process->time_left == 0)
    {
        if(running_process != NULL && running_process->time_left > 0)
        {
            queue_add(running_process, MLF_queues[quantum_current+1]);
        }
        
        running_process = NULL;

        for(int i = 0; i < 4; i++)
        {
            if(MLF_queues[i]->next != NULL)
            {
                running_process = queue_poll(MLF_queues[i]);
                quantum_current = i;
                quantum_remaining_mlf = quantums[i];
                break;
            }
        }
    }

    if(running_process != NULL)
    {
        quantum_remaining_mlf--;
        running_process->time_left--;
    }

    return running_process;
}
int MLF_startup(){

    MLF_queues = (queue_object**)malloc(sizeof(queue_object*) * 4); 
    MLF_queues[0] = new_queue();
    MLF_queues[1] = new_queue();
    MLF_queues[2] = new_queue();
    MLF_queues[3] = new_queue();

    if (MLF_queues[0] == NULL || MLF_queues[1] == NULL || MLF_queues[2] == NULL || MLF_queues[3] == NULL)
    {
        return 1;
    }

    return 0;
}

process* MLF_new_arrival(process* arriving_process, process* running_process){

    if(arriving_process!=NULL)
    {
        queue_add(arriving_process, MLF_queues[0]);
    }

    return running_process;
}

void MLF_finish() {

    free_queue(MLF_queues[0]);
    free_queue(MLF_queues[1]);
    free_queue(MLF_queues[2]);
    free_queue(MLF_queues[3]);

    free(MLF_queues);
}