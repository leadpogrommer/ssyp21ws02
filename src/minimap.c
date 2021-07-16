#include <level.h>
#include <minimap.h>

minimap_data_t* minimap_init(WINDOW* game_window, int radius) {
    minimap_data_t* minimap_data = malloc(sizeof(minimap_data_t));
    minimap_data->radius = radius;
    minimap_data->window = newwin(radius * 2 + 1, radius * 2 + 1, 0, getmaxx(game_window) - radius * 2 - 1);
    minimap_data->player_room = (vector2_t) { .x = -1, .y= -1 };
    return minimap_data;
}
void minimap_destroy(minimap_data_t* minimap_data) {
    delwin(minimap_data->window);
    free(minimap_data);
}

void minimap_draw(minimap_data_t* minimap_data, WINDOW* game_window, level_t* level, player_t* player) {
    werase(minimap_data->window);
    mvwin(minimap_data->window, 0, getmaxx(game_window) - minimap_data->radius * 2 - 1);

    for(int i = 0; i < level->room_grid_size.x; i++) {
        for(int j = 0; j < level->room_grid_size.y; j++) {
            if(level->room_grid[j][i] == NULL) continue;

            vector2_t pos = get_level_position(level, (vector2_t) { .x = i, .y =j }, (vector2_t) { .x = 0, .y = 0 });
            vector2_t pos2 = { .x = pos.x + level->room_grid[j][i]->size.x, .y = pos.y + level->room_grid[j][i]->size.y };
            if(player->pos.x >= pos.x && player->pos.y >= pos.y && player->pos.x <= pos2.x && player->pos.y <= pos2.y) {
                minimap_data->player_room.x = i; minimap_data->player_room.y = j;
            }
        }
    }

    if(minimap_data->player_room.x != -1 && minimap_data->player_room.y != -1) {
        for(int i = -minimap_data->radius; i <= minimap_data->radius; i++) {
            for(int j = -minimap_data->radius; j <= minimap_data->radius; j++) {
                vector2_t room = { .x = minimap_data->player_room.x + i, .y = minimap_data->player_room.y + j };
                if (room.x < 0 || room.y < 0 || room.x >= level->room_grid_size.x || room.y >= level->room_grid_size.y || level->room_grid[room.y][room.x] == NULL) continue;
                for(int k = 0; k < level->connected_rooms->size; k++) {
                    if(equal(level->connected_rooms->data[k].first, room)) {
                        if(equal(level->connected_rooms->data[k].second, (vector2_t) { .x = room.x, .y = room.y + 1 }))
                            mvwaddch(minimap_data->window, j * 2 + minimap_data->radius + 1, i * 2 + minimap_data->radius, '|');
                        else if(equal(level->connected_rooms->data[k].second, (vector2_t) { .x = room.x, .y = room.y - 1 }))
                            mvwaddch(minimap_data->window, j * 2 + minimap_data->radius - 1, i * 2 + minimap_data->radius, '|');
                        else if(equal(level->connected_rooms->data[k].second, (vector2_t) { .x = room.x + 1, .y = room.y }))
                            mvwaddch(minimap_data->window, j * 2 + minimap_data->radius, i * 2 + minimap_data->radius + 1, '-');
                        else if(equal(level->connected_rooms->data[k].second, (vector2_t) { .x = room.x - 1, .y = room.y }))
                            mvwaddch(minimap_data->window, j * 2 + minimap_data->radius, i * 2 + minimap_data->radius - 1, '-');
                    }
                }
                mvwaddch(minimap_data->window, j * 2 + minimap_data->radius, i * 2 + minimap_data->radius, '#');
            }
        }
//        for (int i = minimap_data->player_room.x - 1; i <= minimap_data->player_room.x + 1; i++) {
//            for (int j = minimap_data->player_room.y - 1; j <= minimap_data->player_room.y + 1; j++) {
//                if (i < 0 || j < 0 || i >= level->room_grid_size.x || j >= level->room_grid_size.y || level->room_grid[j][i] == NULL) continue;
//                mvwaddch(minimap_data->window, j, i, '@');
//            }
//        }
    }

    wnoutrefresh(minimap_data->window);
}
