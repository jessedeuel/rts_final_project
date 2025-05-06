#include "scheduler.h"
#include "tasks.h"
#include <vector>

scheduler::scheduler(uint32_t num)
{
    current_task_num = 0;
    max_task_number = num;
    tasks.resize(num);
}

void scheduler::add_task(task_pointer_t new_task)
{
    tasks[current_task_num] = new_task;
    current_task_num++;
}

void scheduler::scheduler_loop()
{
    if (current_task_num > 0)
    {
        for(int i = 0; i < current_task_num; i++)
        {
            (*tasks[i])();
        }
    }
}