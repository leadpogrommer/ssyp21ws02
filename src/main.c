#include <unistd.h>
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include "palette.h"
#include "title_screen.h"
//#include "rich_presence.h"
//#include "thirdparty/discord_game_sdk.h"
#include "hud.h"
#include "render.h"
#include "main.h"
#include <SDL/SDL.h>

SDL_Joystick *joystick;
void init_window() {
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    start_color();
    curs_set(0);
    SDL_InitSubSystem(SDL_INIT_JOYSTICK);
    joystick = SDL_JoystickOpen(0);
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
    game_state->world = init_world();
    game_state->world->player->screen_pos.x = getmaxx(stdscr)/2;
    game_state->world->player->screen_pos.y = getmaxy(stdscr)/2;
    game_state->world->hud = init_hud(game_state->game_window, 3, game_state->world->player, &(game_state->world->level), game_state->palette);
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

void psp_handle_input(){
    static Uint32 prev_time = 0;
    Uint32 time = SDL_GetTicks();
    if(time - prev_time > 100){
        prev_time = time;
    } else{
        return 0;
    }
    if(SDL_JoystickGetButton(joystick, 6))ungetch('s');
    if(SDL_JoystickGetButton(joystick, 7))ungetch('a');
    if(SDL_JoystickGetButton(joystick, 8))ungetch('w');
    if(SDL_JoystickGetButton(joystick, 9))ungetch('d');
    if(SDL_JoystickGetButton(joystick, 10))ungetch('p');
    if(SDL_JoystickGetButton(joystick, 2))ungetch(' ');
}

int SDL_main() {
    chdir("ms0:/");
    srand(time(NULL));

    init_window();

//    rp_init();
//    update_rich_presence_menu();

    title_screen_data menu_main;
    title_screen_init(&menu_main, 2, "New game", "Exit");

    title_screen_data menu_pause;
    title_screen_init(&menu_pause, 2, "Continue", "Exit");

    game_state_t game_state = {
            .game_window = stdscr,
            .state = 2
    };
    set_up_palettes(&game_state);
    wbkgd(stdscr, COLOR_PAIR(game_state.palette->text_pair));


    while (game_state.state != 1) {
        psp_handle_input();
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
                        game_state.state = 1;
                }
                title_screen_draw(stdscr, &menu_main, FALSE);
                break;
            case 3:
                switch (title_screen_handle_input(&menu_pause)) {
                    case 0:
                        game_state.state = 0;
                        break;
                    case 1:
                        game_state.state = 1;
                }
                title_screen_draw(stdscr, &menu_pause, TRUE);
                break;
        }
//        rp_tick();
        SDL_Delay(16);
    }

    destroy_palette(game_state.palette);
    destroy_palette(game_state.light_palette);
    destroy_world(game_state.world);
    destroy_hud(game_state.world->hud);
    endwin();
    return 0;
}
