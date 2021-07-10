#include <unistd.h>
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include "palette.h"
#include "title_screen.h"
#include "rich_presence.h"
#include "thirdparty/discord_game_sdk.h"
#include "hud.h"
#include "render.h"
#include "main.h"
#include "saver.h"

void init_window() {
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    start_color();
    curs_set(0);
}

void set_up_palettes(game_state_t* game_state){

    init_color(23, 750, 750, 750);
    init_color(18, 0, 0, 0); // Nice Black
    init_color(21, 900, 450, 0); // Nice Orange
    init_color(19, 550, 550, 550); // Nice White
    init_color(20, 0, 700, 0); // Nice Green
    init_color(22, 600, 0, 0); // Nice Red

    game_state->palette = init_palette(18, 19, 20, 21, 22, 23);

    init_color(28, 180, 180, 180); // Nice Black
    init_color(31, 1000, 550, 0); // Nice Orange
    init_color(29, 850, 850, 850); // Nice White
    init_color(30, 0, 700, 0); // Nice Green
    init_color(32, 700, 0, 0); // Nice Red

    game_state->light_palette = init_palette(18, 29, 30, 31, 32, 29);
}

void start_up_world(game_state_t* game_state){
    game_state->world = start_new_world();
    game_state->world->player->screen_pos.x = getmaxx(stdscr)/2;
    game_state->world->player->screen_pos.y = getmaxy(stdscr)/2;
    game_state->world->hud = init_hud(game_state->game_window, 3, game_state->world->player, &(game_state->world->current_level), game_state->palette);
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
    render(game_state);
    return 0;
}

int main() {
    srand(time(NULL));

    init_window();

    rp_init();
    update_rich_presence_menu();

    title_screen_data menu_main;
    title_screen_init(&menu_main, 3, "New game", "Load", "Exit");

    title_screen_data menu_pause;
    title_screen_init(&menu_pause, 3, "Continue", "Save", "Exit");

    game_state_t game_state = {
            .game_window = stdscr,
            .state = 2
    };
    set_up_palettes(&game_state);
    wbkgd(stdscr, COLOR_PAIR(game_state.palette->text_pair));

    int n = sizeof(int);

    while (game_state.state != 1) {
        switch(game_state.state) {
            case 0:
                if(!game_state.world){
                    start_up_world(&game_state);
                }

                handle_input(&game_state);
                process(&game_state);
                draw(&game_state);
                break;
            case 2:
                switch (title_screen_handle_input(&menu_main)) {
                    case 0:
                        game_state.state = 0;
                        break;
                    case 1:
                        game_state.world = load_world();
                        game_state.world->hud = init_hud(game_state.game_window, 3, game_state.world->player, &(game_state.world->current_level), game_state.palette);
                        game_state.state = 0;
                        break;
                    case 2:
                        game_state.state = 1;
                        break;
                }
                title_screen_draw(stdscr, &menu_main, FALSE);
                break;
            case 3:
                switch (title_screen_handle_input(&menu_pause)) {
                    case 0:
                        game_state.state = 0;
                        break;
                    case 1:
                        save_world(game_state.world);
                        game_state.state = 0;
                        break;
                    case 2:
                        game_state.state = 1;
                        break;
                }
                title_screen_draw(stdscr, &menu_pause, TRUE);
                break;
        }
        rp_tick();
        usleep(16 * 1000);
    }

    title_screen_destroy(&menu_main);
    title_screen_destroy(&menu_pause);
    destroy_palette(game_state.palette);
    destroy_palette(game_state.light_palette);
    if (game_state.world){
        destroy_world(game_state.world);
    }
    endwin();
    return 0;
}
