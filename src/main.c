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
            move_player(world->player, VEC2_LEFT);
            break;
        case 'd':
            move_player(world->player, VEC2_RIGHT);
            break;
        case 'w':
            move_player(world->player, VEC2_UP);
            break;
        case 's':
            move_player(world->player, VEC2_DOWN);
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

    vector2_t offset = {.x = 0, .y = 0};
    for (int i = 0; i < room_pool->count; i++){
        draw_room(window, palette, room_pool->rooms[i], sum(get_origin_on_screen(world), offset));
        offset.y += room_pool->rooms[i]->height;
    }

    mvwaddch(window, world->player->screen_pos.y, world->player->screen_pos.x, palette->symbol['P']);

    wrefresh(window);
    return 0;
}

int main() {
    init_window();

    palette_t* palette = set_up_palette();

    room_pool_t* room_pool = load_room_directory("resources/rooms");

    world_t* world = init_world();
    world->player->screen_pos.x = getmaxx(stdscr)/2;
    world->player->screen_pos.y = getmaxy(stdscr)/2;

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
