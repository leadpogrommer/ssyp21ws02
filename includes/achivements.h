#pragma once
#include "statistics.h"

typedef struct achievement_t{
    char* title;
    char is_complete;
    char (*evaluate)(statistics_t* statistics);
}achievement_t;

typedef struct achievements_t{
    achievement_t* data;
    int count;
}achievements_t;

achievements_t* init_achievements(statistics_t* statistics);
void destroy_achievements(achievements_t* achievements);

char deaths_achievement_t0(statistics_t* statistics);
char deaths_achievement_t1(statistics_t* statistics);
char deaths_achievement_t2(statistics_t* statistics);
char deaths_achievement_t3(statistics_t* statistics);
char deaths_achievement_t4(statistics_t* statistics);
char kills_achievement_t0(statistics_t* statistics);
char kills_achievement_t1(statistics_t* statistics);
char kills_achievement_t2(statistics_t* statistics);
char kills_achievement_t3(statistics_t* statistics);
char kills_achievement_t4(statistics_t* statistics);