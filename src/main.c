#include <unistd.h>
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include "room.h"
#include "render.h"
#include "world.h"
#include "title_screen.h"
#include "pathfinder.h"
#include "rich_presence.h"

typedef struct {
    palette_t* palette;
    world_t* world;
    int state;
    vector2_t start_point;
    vector2_t end_point;
    vector2_array_t* path;
} game_state_t;

void init_window() {
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    start_color();
    curs_set(0);
    mousemask(BUTTON1_CLICKED | BUTTON3_CLICKED, NULL);
}

palette_t* set_up_palette(){
    init_pair(1, COLOR_CYAN, COLOR_CYAN);

    palette_t* palette = init_palette();
    palette->symbol['.'] = ' ';
    palette->symbol['*'] = ' ' | COLOR_PAIR(1);
    return palette;
}

int handle_input(game_state_t* game_state){
    int input = getch();
    MEVENT event;
    switch (input){
        case 'a':
            move_player_in_world(game_state->world, VEC2_LEFT);
            break;
        case 'd':
            move_player_in_world(game_state->world, VEC2_RIGHT);
            break;
        case 'w':
            move_player_in_world(game_state->world, VEC2_UP);
            break;
        case 's':
            move_player_in_world(game_state->world, VEC2_DOWN);
            break;
        case 'q':
            game_state->state = 1;
            break;
        case 'p':
            game_state->state = 3;
            break;
        case KEY_MOUSE:
            if (getmouse(&event) == OK){
                if (event.bstate & BUTTON1_CLICKED){
                    game_state->start_point = (vector2_t){ .x = event.x, .y = event.y };
                    game_state->start_point = sub(game_state->start_point, get_origin_on_screen(game_state->world));
                }else if (event.bstate & BUTTON3_CLICKED){
                    game_state->end_point = (vector2_t){ .x = event.x, .y = event.y };
                    game_state->end_point = sub(game_state->end_point, get_origin_on_screen(game_state->world));
                }
            }
            break;
        case 'f':
            game_state->path = find_path(game_state->world->pathfinder, game_state->start_point, game_state->end_point);
            break;
        default:
            break;
    }
    return 0;
}


int process(game_state_t* game_state){
    process_world(game_state->world);
    return 0;
}

int draw(WINDOW* window, game_state_t* game_state){
    werase(window);

    vector2_t offset = {.x = 0, .y = 0};
    draw_level(window, game_state->palette, game_state->world->current_level, get_origin_on_screen(game_state->world));

    mvwaddch(window, game_state->world->player->screen_pos.y, game_state->world->player->screen_pos.x, game_state->palette->symbol['P']);

    mvwaddch(window, game_state->start_point.y + get_origin_on_screen(game_state->world).y, game_state->start_point.x + get_origin_on_screen(game_state->world).x, '&');
    mvwaddch(window, game_state->end_point.y + get_origin_on_screen(game_state->world).y, game_state->end_point.x + get_origin_on_screen(game_state->world).x, '&');
    if (game_state->path) {
        for (int i = 0; i < game_state->path->size; i++) {
            vector2_t window_pos = sum(game_state->path->data[i], get_origin_on_screen(game_state->world));
            mvwaddch(window, window_pos.y, window_pos.x, '&');
        }
    }

    wrefresh(window);
    return 0;
}

int main() {
    srand(time(NULL));

    init_window();

    title_screen_data menu_main;
    title_screen_init(&menu_main, 2, "New game", "Exit");

    title_screen_data menu_pause;
    title_screen_init(&menu_pause, 2, "Continue", "Exit");

    game_state_t game_state = {
        .palette = set_up_palette(),
        .world = init_world(),
        .state = 2
    };
    game_state.world->player->screen_pos.x = getmaxx(stdscr)/2;
    game_state.world->player->screen_pos.y = getmaxy(stdscr)/2;

    while (game_state.state != 1) {
        switch(game_state.state) {
            case 0:
                handle_input(&game_state);
                process(&game_state);
                draw(stdscr, &game_state);
                break;
            case 2:
                switch(title_screen_handle_input(&menu_main)) {
                    case 0:
                        game_state.state = 0;
                        break;
                    case 1:
                        game_state.state = 1;
                }
                title_screen_draw(stdscr, &menu_main, FALSE);
                break;
            case 3:
                switch(title_screen_handle_input(&menu_pause)) {
                    case 0:
                        game_state.state = 0;
                        break;
                    case 1:
                        game_state.state = 1;
                }
                title_screen_draw(stdscr, &menu_pause, TRUE);
                break;
        }
        usleep(16 * 1000);
    }

    destroy_palette(game_state.palette);
    destroy_world(game_state.world);
    endwin();
    return 0;
}
