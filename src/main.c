#include <stdio.h>
#include <unistd.h>
#include <ncurses.h>
#include <stdlib.h>
#include "room.h"

int main() {

    room_t* room = load_room("rces/rooms/testroom.txt");
    print_room(room);
    fflush(stdout);
    getchar();

    return 0;
}
