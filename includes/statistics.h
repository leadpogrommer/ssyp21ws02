#pragma once

#define STATISTICS \
        X(enemies_killed) \
        X(shrines_used)   \
        X(max_level)\
        X(deaths)  \
        X(items_picked_up)\
        X(max_gold)\


// TODO we need to use a hash table for stats here
#define X(a) int a;
typedef struct statistics_t{
    STATISTICS
}statistics_t;
#undef X

statistics_t* init_statistics();
void destroy_statistics(statistics_t* stats);