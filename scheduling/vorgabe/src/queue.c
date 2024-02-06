#include "../lib/queue.h"
#include <stdlib.h>
#include <stdio.h>

int queue_add(void* new_object, queue_object* queue){
    
    queue_object* new_queue_object = (queue_object*)malloc(sizeof(queue_object));
    new_queue_object->object = new_object;
    new_queue_object->next = NULL;
    
    if(queue == NULL || queue->next == NULL)
    {
        queue->next = new_queue_object;
    }
    else
    {
        queue_object* current = queue->next;
        
        while(current->next != NULL)
        {
            current = current->next;
        }
        current->next = new_queue_object;
    }

    return 0;
}

void* queue_poll(queue_object* queue){

    if(queue == NULL || queue->next == NULL)
    {
        return NULL;
    }
    
    queue_object* deleted_object = queue->next;
    queue->next = deleted_object->next;
    
    void* object = deleted_object->object;
    free(deleted_object);
    
    return object;
}

queue_object* new_queue(){

    queue_object* queue = (queue_object*)malloc(sizeof(queue_object));
    queue->object = NULL;
    queue->next = NULL;

    return queue;
}


void free_queue(queue_object* queue){

    while(queue->next != NULL)
    {
        queue_poll(queue);
    }
    
    free(queue);
}

void* queue_peek(queue_object* queue){
    
    if(queue == NULL || queue->next == NULL)
    {
        return NULL;
    }
    
    return queue->next->object;
}
