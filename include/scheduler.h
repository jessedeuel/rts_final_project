#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include <vector>

typedef void (*task_pointer_t)();

class scheduler {
public:
    scheduler(const uint32_t num);

    void add_task(task_pointer_t new_task);

    void scheduler_loop();

private:
    uint32_t max_task_number;
    int current_task_num;

    std::vector<task_pointer_t> tasks;
};


#endif