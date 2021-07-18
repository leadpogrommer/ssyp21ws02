#include "achivements.h"
#include "stdlib.h"

achievements_t* init_achievements(statistics_t* statistics){
    int count = 10;
    achievements_t* achievements = calloc(sizeof(achievements_t), 1);
    achievements->count = count;
    achievements->data = calloc(sizeof(achievement_t), achievements->count);
    achievements->data[0] = (achievement_t) {"First try", 0, deaths_achievement_t0};
    achievements->data[1] = (achievement_t) {"Getting started", 0, deaths_achievement_t1};
    achievements->data[2] = (achievement_t) {"You know something", 0, deaths_achievement_t2};
    achievements->data[3] = (achievement_t) {"Experienced", 0, deaths_achievement_t3};
    achievements->data[4] = (achievement_t) {"Stick with it", 0, deaths_achievement_t4};
    achievements->data[5] = (achievement_t) {"First blood", 0, kills_achievement_t0};
    achievements->data[6] = (achievement_t) {"Killing spree", 0, kills_achievement_t1};
    achievements->data[7] = (achievement_t) {"Professional killer", 0, kills_achievement_t2};
    achievements->data[8] = (achievement_t) {"Unstoppable", 0, kills_achievement_t3};
    achievements->data[9] = (achievement_t) {"Greater than the gods", 0, kills_achievement_t4};

    for (int i = 0; i < achievements->count; i++){
        achievements->data[i].is_complete = achievements->data[i].evaluate(statistics);
    }

    return achievements;
}

void destroy_achievements(achievements_t* achievements){
    free(achievements->data);
    free(achievements);
}

char deaths_achievement_t0(statistics_t* statistics){
    return (statistics->deaths >= 1);
}

char deaths_achievement_t1(statistics_t* statistics){
    return (statistics->deaths >= 5);
}

char deaths_achievement_t2(statistics_t* statistics){
    return (statistics->deaths >= 10);
}

char deaths_achievement_t3(statistics_t* statistics){
    return (statistics->deaths >= 25);
}

char deaths_achievement_t4(statistics_t* statistics){
    return (statistics->deaths >= 50);
}

char kills_achievement_t0(statistics_t* statistics){
    return (statistics->enemies_killed >= 1);
}

char kills_achievement_t1(statistics_t* statistics){
    return (statistics->enemies_killed >= 10);
}

char kills_achievement_t2(statistics_t* statistics){
    return (statistics->enemies_killed >= 50);
}

char kills_achievement_t3(statistics_t* statistics){
    return (statistics->enemies_killed >= 150);
}

char kills_achievement_t4(statistics_t* statistics){
    return (statistics->enemies_killed >= 500);
}