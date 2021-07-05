#include "world.h"
#include <stdlib.h>

world_t* init_world(){
    world_t* world = malloc(sizeof(world_t));
    world->player = init_player();
    return world;
}

void destroy_world(world_t* world){
    destroy_player(world->player);
    free(world);
}

