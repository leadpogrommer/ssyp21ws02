#include <ncurses.h>
#include <malloc.h>
#include <string.h>
#include "title_screen.h"
#include "utility.h"
#include "palette.h"

void main_menu_draw(WINDOW* window, int y_offset){
    int x_offset = (getmaxx(window) - 45) / 2;
    mvwprintw(window, y_offset    , x_offset, "┏┓╻┏━╸╻ ╻┏━┓┏━┓┏━╸╺┳┓   ╺┳┓╻ ╻┏┓╻┏━╸┏━╸┏━┓┏┓╻");
    mvwprintw(window, y_offset + 1, x_offset, "┃┗┫┃  ┃ ┃┣┳┛┗━┓┣╸  ┃┃    ┃┃┃ ┃┃┗┫┃╺┓┣╸ ┃ ┃┃┗┫");
    mvwprintw(window, y_offset + 2, x_offset, "╹ ╹┗━╸┗━┛╹┗╸┗━┛┗━╸╺┻┛   ╺┻┛┗━┛╹ ╹┗━┛┗━╸┗━┛╹ ╹");
}

void game_over_draw(WINDOW *window, int y_offset){

    int x_offset = (getmaxx(window) - 24) / 2;

    init_pair(200, COLOR_ENEMY_LIGHT, COLOR_BACKGROUND_DARK);

    wattron(window, COLOR_PAIR(200));
    mvwprintw(window, y_offset    , x_offset, "╻ ╻┏━┓╻ ╻   ╺┳┓╻┏━╸╺┳┓");
    mvwprintw(window, y_offset + 1, x_offset, "┗┳┛┃ ┃┃ ┃    ┃┃┃┣╸  ┃┃");
    mvwprintw(window, y_offset + 2, x_offset, " ╹ ┗━┛┗━┛   ╺┻┛╹┗━╸╺┻┛");
    wattroff(window, COLOR_PAIR(200));
}

void title_screen_init(title_screen_data* data, void (*title_draw)(WINDOW*, int), int count, ...) {
    data->title_draw = title_draw;
    va_list arg;
    va_start(arg, count);

    data->count = count;
    data->pos = 0;
    data->items = malloc(sizeof(char*) * count);
    for(int i = 0; i < count; i++) {
        char* str = va_arg(arg, char*);
        data->items[i] = str;
    }

    va_end(arg);
}

int title_screen_handle_input(title_screen_data* data) {
    switch(getch()) {
        case KEY_DOWN:
        case 's':
            data->pos++;
            if(data->pos >= data->count) data->pos = 0;
            break;
        case KEY_UP:
        case 'w':
            data->pos--;
            if(data->pos < 0) data->pos = data->count - 1;
            break;
        case KEY_ENTER:
        case ' ':
            return data->pos;
    }
    return -1;
}

void title_screen_draw(WINDOW* window, title_screen_data* data, int frame) {
    if(!frame) werase(window);
    else {
        size_t maxw = strlen(data->items[0]);
        for(int i = 1; i < data->count; i++)
            if(strlen(data->items[i]) > maxw)
                maxw = strlen(data->items[i]);
        int offset_x = (getmaxx(window) - maxw - 4) / 2 - 2;
        int offset_y_start = (getmaxy(window) - data->count * 2) / 2 - 2;
        int offset_y_end = offset_y_start + data->count * 2 + 2;
        mvhline(offset_y_start, offset_x, '=', maxw + 4 + 4);
        for(int i = offset_y_start + 1; i < offset_y_end; i++)
            mvhline(i, offset_x, ' ', maxw + 4 + 4);
        mvhline(offset_y_end, offset_x, '=', maxw + 4 + 4);
    }

    int offset_y = (getmaxy(window) - data->count * 2) / 2;
    if(data->title_draw) {
        data->title_draw(window, 3);
    }
    for(int i = 0; i < data->count; i++) {
        int offset_x = (getmaxx(window) - strlen(data->items[i]) - 4) / 2;
        mvwprintw(window, offset_y + i * 2, offset_x, data->pos == i ? "> %s <" : "  %s  ", data->items[i]);
    }

    wnoutrefresh(window);
}

void title_screen_destroy(title_screen_data* data){
    free(data->items);
}
