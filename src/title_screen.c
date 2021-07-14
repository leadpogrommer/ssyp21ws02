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

void title_screen_draw(WINDOW* window, title_screen_data* data, int frame, char* title) {
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
    if(strlen(title) > 0) {
        int offset_x = (getmaxx(window) - strlen(title) - 4) / 2;
        mvwprintw(window, 3, offset_x, "[ %s ]", title);
    }
    for(int i = 0; i < data->count; i++) {
        int offset_x = (getmaxx(window) - strlen(data->items[i]) - 4) / 2;
        mvwprintw(window, offset_y + i * 2, offset_x, data->pos == i ? "> %s <" : "  %s  ", data->items[i]);
    }

    wrefresh(window);
}

void title_screen_destroy(title_screen_data* data){
    free(data->items);
}