#include <unistd.h>
#include <ncurses.h>
#include "room.h"
#include "render.h"
#include "world.h"
#include "player.h"
#include "title_screen.h"
#include "rich_presence.h"

typedef struct {
    palette_t* palette;
    world_t* world;
    room_pool_t* room_pool;
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
    init_pair(1, COLOR_CYAN, COLOR_CYAN);

    palette_t* palette = init_palette();
    palette->symbol['.'] = ' ';
    palette->symbol['*'] = ' ' | COLOR_PAIR(1);
    return palette;
}

int handle_input(game_state_t* game_state){
    char input = getch();
    switch (input){
        case 'a':
            move_player(game_state->world->player, VEC2_LEFT);
            break;
        case 'd':
            move_player(game_state->world->player, VEC2_RIGHT);
            break;
        case 'w':
            move_player(game_state->world->player, VEC2_UP);
            break;
        case 's':
            move_player(game_state->world->player, VEC2_DOWN);
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
    return 0;
}

int draw(WINDOW* window, game_state_t* game_state){
    werase(window);

    vector2_t offset = {.x = 0, .y = 0};
    for (int i = 0; i < game_state->room_pool->count; i++){
        draw_room(window, game_state->palette, game_state->room_pool->rooms[i], sum(get_origin_on_screen(game_state->world), offset));
        offset.y += game_state->room_pool->rooms[i]->height;
    }

    mvwaddch(window, game_state->world->player->screen_pos.y, game_state->world->player->screen_pos.x, game_state->palette->symbol['P']);

    wrefresh(window);
    return 0;
}

int main() {
    rp_init();
    sleep(10);
}
int maina() {
    init_window();

    title_screen_data menu_main;
    title_screen_init(&menu_main, 2, "New game", "Exit");

    title_screen_data menu_pause;
    title_screen_init(&menu_pause, 2, "Continue", "Exit");

    game_state_t game_state = {
        .palette = set_up_palette(),
        .world = init_world(),
        .room_pool = load_room_directory("resources/rooms"),
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
    destroy_room_pool(game_state.room_pool);
    destroy_world(game_state.world);
    endwin();
    return 0;
}
