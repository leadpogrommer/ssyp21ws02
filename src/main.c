#include <unistd.h>
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <minimap.h>
#include "palette.h"
#include "title_screen.h"
#include "rich_presence.h"
#include "thirdparty/discord_game_sdk.h"
#include "hud.h"
#include "render.h"
#include "achivements.h"
#include "main.h"
#include "saver.h"
#include <locale.h>

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

    init_color(COLOR_BACKGROUND_DARK, 100, 100, 100); // Nice Black
    init_color(COLOR_ACTIVITY_DARK, 900, 450, 0); // Nice Orange
    init_color(COLOR_FOREGROUND_DARK, 500, 500, 500); // Nice White
    init_color(COLOR_PLAYER_LIGHT, 0, 700, 0); // Nice Green
    init_color(COLOR_PLAYER_BULLET, 400, 700, 400); // Bullet; Lgreen
    init_color(COLOR_ENEMY_DARK, 400, 0, 0); // Enemy red

    game_state->palette = init_palette(COLOR_BACKGROUND_DARK, COLOR_FOREGROUND_DARK, COLOR_PLAYER_LIGHT, COLOR_ACTIVITY_DARK, COLOR_ENEMY_DARK, COLOR_FOREGROUND_LIGHT, COLOR_PLAYER_BULLET);

    init_color(COLOR_ACTIVITY_LIGHT, 1000, 550, 0); // Nice Orange
    init_color(COLOR_FOREGROUND_LIGHT, 850, 850, 850); // Nice White
    init_color(COLOR_ENEMY_LIGHT, 700, 0, 0); // Nice Red

    game_state->light_palette = init_palette(COLOR_BACKGROUND_DARK, COLOR_FOREGROUND_LIGHT, COLOR_PLAYER_LIGHT, COLOR_ACTIVITY_LIGHT, COLOR_ENEMY_LIGHT, COLOR_FOREGROUND_LIGHT, COLOR_PLAYER_BULLET);

    init_color(COLOR_BACKGROUND_POPUP, 0, 0, 800);

    game_state->level_popup_palette = init_palette(COLOR_BACKGROUND_POPUP, 0, 0, 0, 0, COLOR_FOREGROUND_LIGHT, 0);
}

void set_up_world(game_state_t* game_state){

    game_state->world->hud = init_hud(game_state->game_window, 3, game_state->world->player, &(game_state->world->current_level),
                                      game_state->palette);

    game_state->world->minimap_data = minimap_init(game_state->game_window, 3, 228, 235);

    vector2_t inventory_left_up_padding = { .x = 24, .y = 0 };
    vector2_t inventory_right_down_padding = { .x = 9, .y = game_state->world->hud->height };
    game_state->inventory_display = init_inventory_display(game_state->world->player->inventory, game_state->game_window,
                                                           game_state->palette, inventory_left_up_padding, inventory_right_down_padding);

    if (game_state->achievements){
        destroy_achievements(game_state->achievements);
        destroy_int_array(game_state->achievement_queue);
    }
    game_state->achievements = init_achievements(game_state->world->stats);
    game_state->achievement_queue = init_int_array();
    WINDOW* win = game_state->game_window;
    game_state->world->max_fade_radius = (getmaxx(win) * getmaxx(win) + getmaxy(win) * getmaxy(win) ) / 2;
    // half of the diagonal so it just around the corner
    game_state->world->fade_speed = game_state->world->max_fade_radius / 1600;
}

void end_game(game_state_t* game_state){
    save_statistics(game_state->world->stats);
    destroy_world(game_state->world);
    destroy_inventory_display(game_state->inventory_display);
    if (game_state->achievements){
        destroy_achievements(game_state->achievements);
        destroy_int_array(game_state->achievement_queue);
        game_state->achievements = NULL;
    }
}

int handle_input(game_state_t* game_state){
    int input;

    while ( (input = getch()) != ERR ) {
        switch (input) {
            case 'a':
                if (game_state->state == STATE_GAME) {
                    move_player_in_world(game_state->world, VEC2_LEFT);
                }
                break;
            case 'd':
                if (game_state->state == STATE_GAME) {
                    move_player_in_world(game_state->world, VEC2_RIGHT);
                }
                break;
            case 'w':
                if (game_state->state == STATE_GAME) {
                    move_player_in_world(game_state->world, VEC2_UP);
                }
                break;
            case 's':
                if (game_state->state == STATE_GAME) {
                    move_player_in_world(game_state->world, VEC2_DOWN);
                }
                break;
            case 'e':
                use_shrine(game_state->world);
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
                if (game_state->state == STATE_INVENTORY) {
                    game_state->inventory_display->current_item++;
                } else if (game_state->state == STATE_GAME) {
                    shoot(game_state->world->player, game_state->world->bullets, VEC2_DOWN, game_state->world->time);
                }
                break;
            case KEY_UP:
                if (game_state->state == STATE_INVENTORY) {
                    game_state->inventory_display->current_item--;
                } else if (game_state->state == STATE_GAME) {
                    shoot(game_state->world->player, game_state->world->bullets, VEC2_UP, game_state->world->time);
                }
                break;
            case KEY_RIGHT:
                if (game_state->state == STATE_GAME) {
                    shoot(game_state->world->player, game_state->world->bullets, VEC2_RIGHT, game_state->world->time);
                }
                break;
            case KEY_LEFT:
                if (game_state->state == STATE_GAME) {
                    shoot(game_state->world->player, game_state->world->bullets, VEC2_LEFT, game_state->world->time);
                }
                break;
            case ' ':
                if (game_state->state == STATE_INVENTORY && game_state->world->player->inventory->item_count > 0) {
                    int current_item = game_state->inventory_display->current_item;
                    if (use_item(game_state->world->player, current_item, game_state->world)) {
                        print_message(game_state->world->hud, 0, 1, 120, "You have just used %s",
                                      game_state->world->player->inventory->items[current_item]->name);
                    } else {
                        print_message(game_state->world->hud, 0, 1, 30, "This item can't be used");
                    }
                }
                break;
            default:
                break;
        }
    }
    return 0;
}


int process(game_state_t* game_state){
    process_world(game_state->world);
    if(game_state->world->player->health <= 0){
        game_state->state = STATE_GAMEOVER;
        game_state->world->stats->deaths++;
    }

    // Processing achievements
    for (int i = 0; i < game_state->achievements->count; i++){
        if (!game_state->achievements->data[i].is_complete && game_state->achievements->data[i].evaluate(game_state->world->stats)){
            push_back_int(game_state->achievement_queue, i);
            game_state->achievements->data[i].is_complete = 1;
        }
    }

    if (!game_state->achievement_popup && game_state->achievement_queue->size){
        char* achievement_title = game_state->achievements->data[game_state->achievement_queue->data[0]].title;
        game_state->achievement_popup = init_popup(game_state->game_window, 7, POPUP_ULCORNER,
                                                   "Achievement unlocked!\n%s", achievement_title);
        delete_int_from_array(game_state->achievement_queue, 0);
    }

    if (game_state->achievement_popup){
        process_popup(&game_state->achievement_popup);
    }

    return 0;
}

int draw(game_state_t* game_state){
    render(game_state);
    return 0;
}

int main() {
    setlocale(LC_ALL, "");
    srand(time(NULL));

    init_window();

    rp_init();
    update_rich_presence_menu();

    title_screen_data menu_main;
    title_screen_init(&menu_main, main_menu_draw, 3, "New game", "Load", "Exit");

    title_screen_data menu_pause;
    title_screen_init(&menu_pause, NULL, 3, "Continue", "Main Menu", "Exit");

    title_screen_data menu_gameover;
    title_screen_init(&menu_gameover, game_over_draw, 2, "Main menu", "Exit");

    game_state_t game_state = {
            .game_window = stdscr,
            .state = 2,
            .achievements = NULL
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
                handle_input(&game_state);
                draw(&game_state);
                break;
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
                title_screen_draw(stdscr, &menu_main, FALSE);
                break;
            case STATE_PAUSE_MENU:
                switch (title_screen_handle_input(&menu_pause)) {
                    case 0:
                        game_state.state = STATE_GAME;
                        break;
                    case 1:
                        end_game(&game_state);
                        game_state.world = NULL;
                        game_state.state = STATE_MAIN_MENU;
                        break;
                    case 2:
                        game_state.state = STATE_EXIT;
                        break;
                }
                title_screen_draw(stdscr, &menu_pause, TRUE);
                break;
            case STATE_GAMEOVER:
                switch (title_screen_handle_input(&menu_gameover)) {
                    case 0:
                        end_game(&game_state);
                        game_state.world = NULL;
                        game_state.state = STATE_MAIN_MENU;
                        break;
                    case 1:
                        game_state.state = STATE_EXIT;
                        break;
                }
                title_screen_draw(stdscr, &menu_gameover, FALSE);
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
    destroy_palette(game_state.level_popup_palette);
    if (game_state.world){
        end_game(&game_state);
    }
    endwin();
    return 0;
}
