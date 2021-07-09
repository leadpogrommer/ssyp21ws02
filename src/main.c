#include <unistd.h>
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include "room.h"
#include "render.h"
#include "world.h"
#include "title_screen.h"
#include "rich_presence.h"
#include "hud.h"

typedef struct {
    palette_t* palette;
    world_t* world;
    WINDOW* game_window;

    int state;
} game_state_t;

void init_window() {
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    start_color();
    curs_set(0);
}

palette_t* set_up_palette(){

    init_color(18, 180, 180, 180); // Nice Black
    init_color(21, 1000, 550, 0); // Nice Orange
    init_color(19, 850, 850, 850); // Nice White
    init_color(20, 0, 700, 0); // Nice Green
    init_color(22, 700, 0, 0); // Nice Red

    palette_t* palette = init_palette(18, 19, 20, 21, 22, 19);

    return palette;
}

int handle_input(game_state_t* game_state){
    int input = getch();
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
        default:
            break;
    }
    return 0;
}


int process(game_state_t* game_state){
    process_world(game_state->world);
    return 0;
}

int draw(game_state_t* game_state){
    werase(game_state->game_window);
    //fill_window_with_background_color(game_state->game_window, game_state->palette);

    draw_level(game_state->game_window, game_state->palette, game_state->world->current_level, get_origin_on_screen(game_state->world));

    mvwaddch(game_state->game_window, game_state->world->player->screen_pos.y, game_state->world->player->screen_pos.x, game_state->palette->symbol['P']);

    wnoutrefresh(game_state->game_window);

    draw_hud(game_state->world->hud);
    doupdate();
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
            .game_window = stdscr,
            .state = 2
    };
    game_state.world->player->screen_pos.x = getmaxx(stdscr)/2;
    game_state.world->player->screen_pos.y = getmaxy(stdscr)/2;
    game_state.world->hud = init_hud(game_state.game_window, 3, game_state.world->player, &(game_state.world->level), game_state.palette);
    wbkgd(stdscr, COLOR_PAIR(game_state.palette->text_pair));


    while (game_state.state != 1) {
        switch(game_state.state) {
            case 0:
                handle_input(&game_state);
                process(&game_state);
                draw(&game_state);
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
    destroy_hud(game_state.world->hud);
    endwin();
    return 0;
}
