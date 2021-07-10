#pragma once
#include "world.h"
#include "main.h"
#include "thirdparty/discord_game_sdk.h"

struct Application {
    struct IDiscordCore* core;
    struct IDiscordUsers* users;
};

void rp_init();
void rp_update(struct DiscordActivity* activity);
void rp_tick();

void update_rich_presence_level(int level);
void update_rich_presence_menu();