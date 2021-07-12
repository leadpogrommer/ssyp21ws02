#pragma once
#include "statistics.h"

typedef struct achivement_t{
    char is_complete;
    char (*evaluate)(statistics_t* statistics);
}achivement_t;