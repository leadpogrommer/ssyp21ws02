#include "render.h"
#include <string.h>


void render(game_state_t* game_state){
    werase(game_state->game_window);

    draw_level_with_lighting(game_state->game_window, game_state->palette, game_state->light_palette, game_state->world, get_origin_on_screen(game_state->world));

    draw_player(game_state->game_window, game_state->world->player, game_state->light_palette);

    wnoutrefresh(game_state->game_window);

    draw_hud(game_state->world->hud);
    doupdate();
}

void draw_level(WINDOW* window, palette_t* palette, level_t* level, vector2_t offset){
    for (int i = 0; i < level->size.y; i++){
        for (int j = 0; j < level->size.x; j++){
            if (!level->data[i][j]){
                mvwaddch(window, i + offset.y, j + offset.x, palette->symbol['*']);
            }else{
                mvwaddch(window, i + offset.y, j + offset.x, palette->symbol[level->data[i][j]]);
            }
        }
    }
}

void mark_visible_symbols_on_line(world_t* world, vector2_t end, short is_visible[world->level->size.y][world->level->size.x]){
    vector2_t diff = sub(end, world->player->pos);
    vector2_t abs_diff = { .y = diff.y > 0 ? diff.y : -diff.y,  .x = diff.x > 0 ? diff.x : -diff.x};
    int y_step = diff.y > 0 ? 1 : diff.y == 0 ? 0 : -1;
    int x_step = diff.x > 0 ? 1 : diff.x == 0 ? 0 : -1;

    vector2_t current = world->player->pos;
    int* bigger = &(current.x);
    int* smaller = &(current.y);
    int bigger_step = x_step;
    int smaller_step = y_step;
    if (abs_diff.y > abs_diff.x){
        bigger_step = y_step;
        smaller_step = x_step;
        abs_diff = (vector2_t) { .x = abs_diff.y, .y = abs_diff.x };
        bigger = &(current.y);
        smaller = &(current.x);
    }

    int error = 0;
    short visible = 1;
    int derror = (abs_diff.y);
    for (int i = 0; i < world->player->vision_radius && visible; i++){
        *bigger += bigger_step;
        error += derror;
        if (error >= abs_diff.x){
            error -= abs_diff.x;
            *smaller += smaller_step;
        }

        if (world->level->data[current.y][current.x] == 0 ||
            world->level->data[current.y][current.x] == '*'){
            visible = 0;
        }
        is_visible[current.y][current.x] = 1;

    }
}

void draw_level_with_lighting(WINDOW* window, palette_t* palette, palette_t* light_palette, world_t* world, vector2_t offset){
    short is_visible[world->level->size.y][world->level->size.x];
    memset(is_visible, 0, sizeof(is_visible));

    for (int i = 0; i < world->level->size.y; i++){
         vector2_t level_left = { .x = 0, .y = i };
         vector2_t level_right = { .x = world->level->size.x - 1, .y = i };
         mark_visible_symbols_on_line(world, level_left, is_visible);
         mark_visible_symbols_on_line(world, level_right, is_visible);
    }

    for (int i = 0; i < world->level->size.x; i++){
        vector2_t level_left = { .x = i, .y = 0 };
        vector2_t level_right = { .x = i, .y = world->level->size.y - 1 };
        mark_visible_symbols_on_line(world, level_left, is_visible);
        mark_visible_symbols_on_line(world, level_right, is_visible);
    }

    for (int i = 0; i < world->level->size.y; i++){
        for (int j = 0; j < world->level->size.x; j++){
            char character = world->level->data[i][j] ? world->level->data[i][j] : '*';
            if (is_visible[i][j]){
                mvwaddch(window, i + offset.y, j + offset.x, light_palette->symbol[character]);
            }else{
                mvwaddch(window, i + offset.y, j + offset.x, palette->symbol[character]);
            }
        }
    }
}

void draw_room(WINDOW* window, palette_t* palette, room_t* room, vector2_t offset){
    for (int i = 0; i < room->size.y; i++){
        for (int j = 0; j < room->size.x; j++){
            mvwaddch(window, offset.y + i, offset.x + j, palette->symbol[room->data[i][j]]);
        }
    }
}

void draw_hud(hud_t* hud){
    mvwin(hud->window, getmaxy(hud->game_window) - hud->height, 0);

    //wattron(hud->window, COLOR_PAIR(hud->palette->text_pair));
    werase(hud->window);

    box(hud->window, 0, 0);
    mvwprintw(hud->window, 1, 1, "gold: %d", hud->player->gold);
    mvwprintw(hud->window, 1, 12, "hp: %d/%d", hud->player->health, hud->player->max_health);
    mvwprintw(hud->window, 1, 24, hud->message);
    mvwprintw(hud->window, 1, getmaxx(hud->window) - 9, "lvl: %d", *(hud->current_level));

    //wattroff(hud->window, COLOR_PAIR(hud->palette->text_pair));

    wnoutrefresh(hud->window);
}

void draw_player(WINDOW* window, player_t* player, palette_t* palette){
    mvwaddch(window, player->screen_pos.y, player->screen_pos.x, palette->symbol['P']);
}