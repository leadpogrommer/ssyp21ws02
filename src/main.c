#include <stdio.h>
#include <unistd.h>
#include <ncurses.h>
#include <stdlib.h>
#include "room.h"
#include "render.h"
#include "world.h"
#include "player.h"

void init_window(){
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    start_color();
    curs_set(0);
}

palette_t* set_up_palette(){
    init_pair(1, COLOR_CYAN, COLOR_CYAN);

    palette_t* palette = init_palette();
    palette->symbol['.'] = ' ';
    palette->symbol['*'] = ' ' | COLOR_PAIR(1);
    return palette;
}

int handle_input(world_t* world){
    char input = getch();
    switch (input){
        case 'a':
            move_player(world->player, -1, 0);
            break;
        case 'd':
            move_player(world->player, 1, 0);
            break;
        case 'w':
            move_player(world->player, 0, -1);
            break;
        case 's':
            move_player(world->player, 0, 1);
            break;
        case 'q':
            return 1;
        default:
            break;
    }
    return 0;
}

int process(world_t* world){
    return 0;
}

int draw(WINDOW* window, world_t* world, palette_t* palette, room_pool_t* room_pool){
    werase(window);

    int y_offset = 0;
    for (int i = 0; i < room_pool->count; i++){
        draw_room(window, palette, room_pool->rooms[i], -world->player->x + world->player->x_screen, -world->player->y + world->player->y_screen + y_offset);
        y_offset += room_pool->rooms[i]->height;
    }

    mvwaddch(window, world->player->y_screen, world->player->x_screen, palette->symbol['P']);

    wrefresh(window);
    return 0;
}

int main() {
    init_window();

    palette_t* palette = set_up_palette();

    room_pool_t* room_pool = load_room_directory("resources/rooms");

    world_t* world = init_world();
    world->player->x_screen = getmaxx(stdscr)/2;
    world->player->y_screen = getmaxy(stdscr)/2;

    int state = 0;
    while (state == 0){
        state = 0;
        state |= handle_input(world);
        state |= process(world);
        state |= draw(stdscr, world, palette, room_pool);
        usleep(16 * 1000);
    }

    destroy_palette(palette);
    destroy_room_pool(room_pool);
    destroy_world(world);
    endwin();
    return 0;
}
