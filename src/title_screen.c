#include <ncurses.h>
#include <malloc.h>
#include <string.h>
#include "title_screen.h"

void title_screen_init(title_screen_data* data, int count, ...) {
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

void title_screen_draw(WINDOW* window, title_screen_data* data) {
    werase(window);

    int offset_y = (getmaxy(window) - data->count * 2) / 2;
    for(int i = 0; i < data->count; i++) {
        int offset_x = (getmaxx(window) - strlen(data->items[i]) - (data->pos == i ? 4 : 0)) / 2;
        mvwprintw(window, offset_y + i * 2, offset_x, data->pos == i ? "> %s <" : "%s", data->items[i]);
    }

    wrefresh(window);
}