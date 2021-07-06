#pragma once

typedef struct {
    char** items;
    int count;
    int pos;
} title_screen_data;

void title_screen_init(title_screen_data* data, int count, ...);
int title_screen_handle_input(title_screen_data* data);
void title_screen_draw(WINDOW* window, title_screen_data* data);