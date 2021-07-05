#include <stdio.h>
#include <unistd.h>
#include <ncurses.h>
#include <stdlib.h>
#include "room.h"

int main() {

    room_pool_t* room_pool = load_room_directory("resources/rooms");

    for (int i = 0; i < room_pool->count; i++) {
        print_room(room_pool->rooms[i]);
        putchar('\n');
    }
    fflush(stdout);
    getchar();

    return 0;
}
