#include "hud.h"
#include <stdlib.h>

hud_t* init_hud(WINDOW* game_window, int height, player_t* player, int* current_level, palette_t* palette){
    hud_t* hud = malloc(sizeof(hud_t));

    vector2_t window_size = { .y = height, .x = getmaxx(game_window) };
    vector2_t window_pos = { .y = getmaxy(game_window) - height, .x = 0 };
    hud->game_window = game_window;
    hud->height = height;
    hud->window = newwin(window_size.y, window_size.x, window_pos.y, window_pos.x);
    hud->player = player;
    hud->current_level = current_level;
    hud->palette = palette;
    hud->message[0] = 0;

    wbkgd(hud->window, COLOR_PAIR(hud->palette->text_pair));

    fill_window_with_background_color(hud->window, palette);

    return hud;
}

void destroy_hud(hud_t* hud){
    free(hud);
}

void process_hud(hud_t* hud, unsigned long time){
    if (time - hud->last_message > hud->message_delay){
        hud->message[0] = 0;
    }
}

void print_message(hud_t* hud, unsigned long time, char ignore_timeout, unsigned long delay_after, const char* format_string, ...){

    if (time - hud->last_message < hud->message_delay && !ignore_timeout){
        return;
    }

    va_list args;
    va_start(args, format_string);

    vsprintf(hud->message, format_string, args);

    va_end(args);

    hud->last_message = time;
    hud->message_delay = delay_after;
}

inventory_display_t* init_inventory_display(inventory_t* inventory, WINDOW* game_window, palette_t* palette, vector2_t left_up_padding, vector2_t right_down_padding){
    inventory_display_t* display = calloc(sizeof(inventory_display_t), 1);

    display->inventory = inventory;
    display->game_window = game_window;
    display->palette = palette;

    vector2_t display_size = { .y = getmaxy(game_window) - left_up_padding.y - right_down_padding.y,
                               .x = getmaxx(game_window) - left_up_padding.x - right_down_padding.x};
    display->window = newwin(display_size.y, display_size.x, left_up_padding.y, left_up_padding.x);
    display->left_up_corner = left_up_padding;
    display->right_down_corner = right_down_padding;

    wbkgd(display->window, COLOR_PAIR(palette->text_pair));
    return display;
}

void destroy_inventory_display(inventory_display_t* display){
    free(display);
}
