#include <stdio.h>
#include <unistd.h>
#include <ncurses.h>
#include <stdlib.h>
#include "room.h"

void init_window(){
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
}

int main() {
    init_window();

    room_pool_t* room_pool = load_room_directory("resources/rooms");

    int y_offset = 0;
    for (int i = 0; i < room_pool->count; i++) {
        draw_room(stdscr, room_pool->rooms[i], 0, y_offset);
        y_offset += room_pool->rooms[i]->height;
    }
    wrefresh(stdscr);

    while (getch() != 'q'){
        usleep(16 * 1000);
    }

    endwin();
    return 0;
}
