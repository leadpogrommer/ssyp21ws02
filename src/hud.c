#include "hud.h"
#include <stdlib.h>

hud_t* init_hud(WINDOW* game_window, int height, player_t* player, int* current_level, palette_t* palette){
    hud_t* hud = malloc(sizeof(hud_t));

    vector2_t window_size = { .y = height, .x = getmaxx(game_window) };
    vector2_t window_pos = { .y = getmaxy(game_window) - height, .x = 0 };
    hud->window = newwin(window_size.y, window_size.x, window_pos.y, window_pos.x);
    hud->player = player;
    hud->current_level = current_level;
    hud->palette = palette;

    fill_window_with_background_color(hud->window, palette);

    return hud;
}

void destroy_hud(hud_t* hud){
    free(hud);
}

void draw_hud(hud_t* hud){
    wattron(hud->window, COLOR_PAIR(hud->palette->text_pair));

    box(hud->window, 0, 0);
    mvwprintw(hud->window, 1, 1, "gold: %d", hud->player->gold);
    mvwprintw(hud->window, 1, 12, "hp: %d/%d", hud->player->health, hud->player->max_health);
    mvwprintw(hud->window, 1, getmaxx(hud->window) - 9, "lvl: %d", *(hud->current_level));

    wattroff(hud->window, COLOR_PAIR(hud->palette->text_pair));

    wnoutrefresh(hud->window);
}

void print_message(hud_t* hud, const char* format_string, ...){
    // Just to clear the previous message
    for (int i = 24; i < 71; i++){
        wmove(hud->window, 1, i);
        waddch(hud->window, ' ' | COLOR_PAIR(hud->palette->wall_pair));
    }

    va_list args;
    va_start(args, format_string);

    wattron(hud->window, COLOR_PAIR(hud->palette->text_pair));
    wmove(hud->window, 1, 24);
    vw_printw(hud->window, format_string, args);
    wattroff(hud->window, COLOR_PAIR(hud->palette->text_pair));

    va_end(args);
}