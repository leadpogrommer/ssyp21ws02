#pragma once
#include <ncurses.h>
#include "utility.h"

#define POPUP_ULCORNER 1
#define POPUP_URCORNER 2

typedef struct popup_t{
    WINDOW* window;
    int progress, speed, move_fraction, live_time;
    char* message;
    vector2_t message_size;
}popup_t;

popup_t* init_popup(WINDOW* main_window, int speed, char position, const char* format_string, ...);
void destroy_popup(popup_t* popup);
void process_popup(popup_t** popup);