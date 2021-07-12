#include "render.h"
#include <string.h>

void render(game_state_t* game_state){
    werase(game_state->game_window);

    draw_level_with_lighting(game_state->game_window, game_state->palette, game_state->light_palette, game_state->world, get_origin_on_screen(game_state->world));

    draw_player(game_state->game_window, game_state->world->player, game_state->light_palette);

    wnoutrefresh(game_state->game_window);

    if (game_state->state == STATE_INVENTORY){
        draw_inventory(game_state->inventory_display);
    }

    draw_hud(game_state->world->hud);

    if (game_state->world->level_popup){
        draw_popup(game_state->world->level_popup, game_state->level_popup_palette, game_state->palette);
    }
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

    for(int i = 0; i < world->enemies->count; i++) {
        int x = world->enemies->array[i].pos.x;
        int y = world->enemies->array[i].pos.y;
        char ch = world->enemies->array[i].damage == 1 ? 'W' : 'T';
        mvwaddch(window, y + offset.y, x + offset.x,(is_visible[y][x] ? light_palette : palette)->symbol[ch]);
    }
    for(int i = 0; i < world->bullets->count; i++) {
        int x = world->bullets->array[i].pos.x;
        int y = world->bullets->array[i].pos.y;
        mvwaddch(window, y + offset.y, x + offset.x, (is_visible[y][x] ? light_palette : palette)->symbol['-']);
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

    werase(hud->window);

    box(hud->window, 0, 0);
    mvwprintw(hud->window, 1, 1, "gold: %d", hud->player->gold);
    mvwprintw(hud->window, 1, 12, "hp: %d/%d", hud->player->health, hud->player->max_health);
    mvwprintw(hud->window, 1, 24, hud->message);
    mvwprintw(hud->window, 1, getmaxx(hud->window) - 9, "lvl: %d", *(hud->current_level));

    wnoutrefresh(hud->window);
}

void draw_player(WINDOW* window, player_t* player, palette_t* palette){
    player->screen_pos.x = getmaxx(window)/2;
    player->screen_pos.y = getmaxy(window)/2;

    mvwaddch(window, player->screen_pos.y, player->screen_pos.x, palette->symbol['P']);
}

void draw_inventory(inventory_display_t* display){
    werase(display->window);


    vector2_t display_size = { .y = getmaxy(display->game_window) - display->left_up_corner.y - display->right_down_corner.y,
                               .x = getmaxx(display->game_window) - display->left_up_corner.x - display->right_down_corner.x};
    wresize(display->window, display_size.y, display_size.x);

    box(display->window, 0, 0);

    vector2_t half;
    getmaxyx(display->window, half.y, half.x);
    half = scaledown(half, 2);
    display->current_item = display->current_item >= display->inventory->item_count ? display->inventory->item_count - 1
                            : (display->current_item < 0 ? 0 : display->current_item);

    mvwprintw(display->window, 1, half.x - 4, "INVENTORY");
    for (int i = 0; i < display->inventory->item_count; i++){
        if (display->current_item == i){
            mvwprintw(display->window, 3 + i, 2, "%s <", display->inventory->items[i]->name);
        }else{
            mvwprintw(display->window, 3 + i, 2, "%s", display->inventory->items[i]->name);
        }
    }

    wnoutrefresh(display->window);
}
void draw_popup(popup_t* popup, palette_t* popup_palette, palette_t* main_palette){
    wbkgd(popup->window, COLOR_PAIR(main_palette->text_pair));
    werase(popup->window);

    int portion;
    if (popup->progress > 100 - popup->move_fraction){
        portion = 100 * ((double)(100 - popup->progress) / popup->move_fraction);
    }else if (popup->progress > popup->move_fraction){
        portion = 100;
    }else {
        portion = 100 * ((double)(popup->progress) / popup->move_fraction);
    }

    int text_position = getmaxy(popup->window) + 1;
    text_position *= portion;
    text_position /= 100;
    text_position -= getmaxy(popup->window);


    wattron(popup->window, COLOR_PAIR(popup_palette->text_pair));

    // filling all the space with color
    for (int i = 0; i < popup->message_size.y; i++){
        mvwhline(popup->window,text_position + i, 1, ' ', popup->message_size.x);
    }

    mvwprintw(popup->window, text_position, 1, popup->message);

    // drawing moving box around text
    mvwhline(popup->window,text_position + popup->message_size.y, 1, ACS_HLINE, popup->message_size.x);
    mvwhline(popup->window,text_position - 1, 1, ACS_HLINE, popup->message_size.x);
    mvwvline(popup->window, text_position < 0 ? 0 : text_position, 0, ACS_VLINE, popup->message_size.y + text_position);
    mvwvline(popup->window, text_position < 0 ? 0 : text_position, getmaxx(popup->window) - 1, ACS_VLINE, popup->message_size.y + text_position);
    mvwaddch(popup->window, text_position - 1, 0, ACS_ULCORNER);
    mvwaddch(popup->window, text_position - 1, popup->message_size.x + 1, ACS_URCORNER);
    mvwaddch(popup->window, text_position + popup->message_size.y, 0, ACS_LLCORNER);
    mvwaddch(popup->window, text_position + popup->message_size.y, popup->message_size.x + 1, ACS_LRCORNER);

    wattroff(popup->window,  COLOR_PAIR(popup_palette->text_pair));

    wnoutrefresh(popup->window);
}