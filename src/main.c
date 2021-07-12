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
    init_color(19, 500, 500, 500); // Nice White
    init_color(20, 0, 700, 0); // Nice Green
    init_color(22, 600, 0, 0); // Nice Red
    init_color(33, 400, 700, 400); // Bullet; Lgreen

    game_state->palette = init_palette(18, 19, 20, 21, 22, 23, 33);

    init_color(28, 180, 180, 180); // Nice Black
    init_color(31, 1000, 550, 0); // Nice Orange
    init_color(29, 850, 850, 850); // Nice White
    init_color(30, 0, 700, 0); // Nice Green
    init_color(32, 700, 0, 0); // Nice Red

    game_state->light_palette = init_palette(18, 29, 30, 31, 32, 29, 33);
}

void set_up_world(game_state_t* game_state){

    game_state->world->hud = init_hud(game_state->game_window, 3, game_state->world->player, &(game_state->world->current_level),
                                      game_state->palette);

    vector2_t inventory_left_up_padding = { .x = 24, .y = 0 };
    vector2_t inventory_right_down_padding = { .x = 9, .y = game_state->world->hud->height };
    game_state->inventory_display = init_inventory_display(game_state->world->player->inventory, game_state->game_window,
                                                           game_state->palette, inventory_left_up_padding, inventory_right_down_padding);
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
            game_state->state = STATE_EXIT;
            break;
        case 'p':
            game_state->state = STATE_PAUSE_MENU;
            break;
        case 'i':
            game_state->state = (game_state->state == STATE_GAME ? STATE_INVENTORY : STATE_GAME);
            break;
        case KEY_DOWN:
            if (game_state->state == STATE_INVENTORY){
                game_state->inventory_display->current_item++;
            } else if(game_state->state == STATE_GAME) {
                fire(game_state->world->bullets, game_state->world->player, VEC2_DOWN);
            }
            break;
        case KEY_UP:
            if (game_state->state == STATE_INVENTORY){
                game_state->inventory_display->current_item--;
            } else if(game_state->state == STATE_GAME) {
                fire(game_state->world->bullets, game_state->world->player, VEC2_UP);
            }
            break;
        case KEY_RIGHT:
            if(game_state->state == STATE_GAME) {
                fire(game_state->world->bullets, game_state->world->player, VEC2_RIGHT);
            }
            break;
        case KEY_LEFT:
            if(game_state->state == STATE_GAME) {
                fire(game_state->world->bullets, game_state->world->player, VEC2_LEFT);
            }
        case ' ':
            if (game_state->state == STATE_INVENTORY){
                item_t* current_item = game_state->world->player->inventory->items[game_state->inventory_display->current_item];
                if (use_item(game_state->world->player, current_item, game_state->world)){
                    // Some stuff
                }else{
                    print_message(game_state->world->hud, "This item can't be used");
                }
            }
            break;
        default:
            break;
    }
    return 0;
}


int process(game_state_t* game_state){
    process_world(game_state->world);
    if(game_state->world->player->health <= 0) game_state->state = STATE_GAMEOVER;
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

    title_screen_data menu_gameover;
    title_screen_init(&menu_gameover, 2, "Main menu", "Exit");

    game_state_t game_state = {
            .game_window = stdscr,
            .state = 2
    };
    set_up_palettes(&game_state);
    wbkgd(stdscr, COLOR_PAIR(game_state.palette->text_pair));

    while (game_state.state != STATE_EXIT) {
        switch(game_state.state) {
            case STATE_GAME:
                if(!game_state.world){
                    game_state.world = start_new_world();
                    set_up_world(&game_state);
                }
                process(&game_state);
            case STATE_INVENTORY:
                handle_input(&game_state);
                draw(&game_state);
                break;
            case STATE_MAIN_MENU:
                switch (title_screen_handle_input(&menu_main)) {
                    case 0:
                        game_state.state = STATE_GAME;
                        break;
                    case 1:
                        game_state.world = load_world();
                        set_up_world(&game_state);
                        game_state.state = STATE_GAME;
                        break;
                    case 2:
                        game_state.state = STATE_EXIT;
                        break;
                }
                title_screen_draw(stdscr, &menu_main, FALSE, "");
                break;
            case STATE_PAUSE_MENU:
                switch (title_screen_handle_input(&menu_pause)) {
                    case 0:
                        game_state.state = STATE_GAME;
                        break;
                    case 1:
                        save_world(game_state.world);
                        game_state.state = STATE_GAME;
                        break;
                    case 2:
                        game_state.state = STATE_EXIT;
                        break;
                }
                title_screen_draw(stdscr, &menu_pause, TRUE, "");
                break;
            case STATE_GAMEOVER:
                switch (title_screen_handle_input(&menu_gameover)) {
                    case 0:
                        destroy_world(game_state.world);
                        game_state.world = NULL;
                        game_state.state = STATE_MAIN_MENU;
                        break;
                    case 1:
                        game_state.state = STATE_EXIT;
                        break;
                }
                title_screen_draw(stdscr, &menu_gameover, FALSE, "Game over");
                break;
        }
        rp_tick();
        usleep(16 * 1000);
    }

    title_screen_destroy(&menu_main);
    title_screen_destroy(&menu_pause);
    title_screen_destroy(&menu_gameover);
    destroy_palette(game_state.palette);
    destroy_palette(game_state.light_palette);
    if (game_state.world){
        destroy_world(game_state.world);
        destroy_inventory_display(game_state.inventory_display);
    }
    endwin();
    return 0;
}
