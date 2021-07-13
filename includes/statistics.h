#pragma once

typedef struct statistics_t{
    int enemies_killed, shrines_used, max_level, deaths, items_picked_up, max_gold;
}statistics_t;

statistics_t* init_statistics();
void destroy_statistics(statistics_t* stats);