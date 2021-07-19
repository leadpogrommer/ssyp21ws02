#include "discord_game_sdk.h"
#include "rich_presence.h"
#include <stdio.h>
#include <time.h>

static struct Application app;

void rp_init() {
    memset(&app, 0, sizeof(app));

    struct DiscordCreateParams params;
    params.client_id = 861915127292231691l;
    params.flags = DiscordCreateFlags_NoRequireDiscord;
    params.event_data = &app;
    params.events = 0;

    DiscordCreate(DISCORD_VERSION, &params, &app.core);
}

void rp_update(struct DiscordActivity *activity) {
    if (app.core){
        app.core->get_activity_manager(app.core)->update_activity(app.core->get_activity_manager(app.core), activity, 0, 0);
    }
}

void rp_tick() {
    if (app.core){
        app.core->run_callbacks(app.core);
    }
}

void update_rich_presence_level(int level) {
    struct DiscordActivity activity = {.assets.large_image = "logo"};
    sprintf(activity.details, "Playing on level %d", level);
    activity.timestamps.start = time(NULL);
    rp_update(&activity);
}

void update_rich_presence_menu() {
    struct DiscordActivity activity = {.assets.large_image = "logo"};
    sprintf(activity.details, "In main menu");
    rp_update(&activity);
}
