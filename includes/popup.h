#pragma once
#include <ncurses.h>
#include "utility.h"

typedef struct popup_t{
    WINDOW* window;
    int progress, speed, move_fraction;
    char* message;
    vector2_t message_size;
}popup_t;

popup_t* init_popup(char* message, int speed);
void destroy_popup(popup_t* popup);
void process_popup(popup_t** popup);