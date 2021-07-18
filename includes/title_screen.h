#pragma once

typedef struct vector2_t vector2_t;

typedef struct {
    char** items;
    int count;
    int pos;
    void (*title_draw)(WINDOW *, int);
} title_screen_data;

void title_screen_init(title_screen_data* data, void (*title_draw)(WINDOW*, int), int count, ...);
int title_screen_handle_input(title_screen_data* data);
void title_screen_draw(WINDOW* window, title_screen_data* data, int frame);
void title_screen_destroy(title_screen_data* data);

void main_menu_draw(WINDOW* window, int y_offset);
void game_over_draw(WINDOW *window, int y_offset);
