#include <unistd.h>
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include "room.h"
#include "render.h"
#include "world.h"

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
            move_player_in_world(world, VEC2_LEFT);
            break;
        case 'd':
            move_player_in_world(world, VEC2_RIGHT);
            break;
        case 'w':
            move_player_in_world(world, VEC2_UP);
            break;
        case 's':
            move_player_in_world(world, VEC2_DOWN);
            break;
        case 'q':
            return 1;
        default:
            break;
    }
    return 0;
}

int process(world_t* world){
    process_world(world);
    return 0;
}

int draw(WINDOW* window, world_t* world, palette_t* palette){
    werase(window);

    vector2_t offset = {.x = 0, .y = 0};
    draw_level(window, palette, world->current_level, get_origin_on_screen(world));

    mvwaddch(window, world->player->screen_pos.y, world->player->screen_pos.x, palette->symbol['P']);

    wrefresh(window);
    return 0;
}

int main() {
    srand(time(NULL));

    init_window();

    palette_t* palette = set_up_palette();

    world_t* world = init_world();
    world->player->screen_pos.x = getmaxx(stdscr)/2;
    world->player->screen_pos.y = getmaxy(stdscr)/2;

    int state = 0;
    while (state == 0){
        state = 0;
        state |= handle_input(world);
        state |= process(world);
        state |= draw(stdscr, world, palette);
        usleep(16 * 1000);
    }

    destroy_palette(palette);
    destroy_world(world);
    endwin();
    return 0;
}
