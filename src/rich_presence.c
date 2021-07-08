#include "thirdparty/discord_game_sdk.h"
#include "rich_presence.h"

static struct Application app;

void rp_init() {
    memset(&app, 0, sizeof(app));

    struct DiscordCreateParams params;
    params.client_id = 861915127292231691l;
    params.flags = DiscordCreateFlags_Default;
    params.event_data = &app;
    params.events = 0;

    DiscordCreate(DISCORD_VERSION, &params, &app.core);
}

void rp_update(struct DiscordActivity *activity) {
    app.core->get_activity_manager(app.core)->update_activity(app.core->get_activity_manager(app.core), activity, 0, 0);
}

void rp_tick() {
    app.core->run_callbacks(app.core);
}