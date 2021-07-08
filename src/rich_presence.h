#pragma once

#include "thirdparty/discord_game_sdk.h"

struct Application {
    struct IDiscordCore* core;
    struct IDiscordUsers* users;
};

void rp_init();
void rp_update(struct DiscordActivity* activity);
void rp_tick();