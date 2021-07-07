#include "thirdparty/discord_game_sdk.h"
#include "rich_presence.h"

void rp_init() {
    struct Application {
        struct IDiscordCore* core;
        struct IDiscordUsers* users;
    };

    struct Application app;
    memset(&app, 0, sizeof(app));

    struct DiscordCreateParams params;
    params.client_id = "861915127292231691";
    params.flags = DiscordCreateFlags_Default;
    params.event_data = &app;

    DiscordCreate(DISCORD_VERSION, &params, &app.core);

    struct DiscordActivity activity = {
            .application_id = "861915127292231691",
            .name = "ncurses_game",
            .state = "hello world",
            .details = "test"
    };
    struct IDiscordActivityManager* activity_manager = app.core->get_activity_manager(&app.core);
    activity_manager->update_activity(activity_manager, &activity, 0, 0);
}