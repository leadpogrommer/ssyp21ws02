#include "statistics.h"
#include <stdlib.h>

statistics_t* init_statistics(){
    statistics_t* stats = calloc(sizeof(statistics_t), 1);
    return stats;
}

void destroy_statistics(statistics_t* stats){
    free(stats);
}
