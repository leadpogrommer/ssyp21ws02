#include <stdio.h>
#include <unistd.h>
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) < (b) ? (b) : (a))

void setUpWindow() {
    initscr();

    cbreak();
    noecho();
    curs_set(0);
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
}

typedef struct obstacle_t{
    int pos;
}obstacle_t;

typedef struct dino_t{
    int distanceToGround;
    int jumpHeight;
    int jumpTime;   // in frames
    int isJumping;
    int position;
}dino_t;

typedef struct world_t{
    obstacle_t* obstacles;
    int obstaclesCount;
    int farthestObstacleIndex;
    int obstaclesMoveTime; // in frames
    dino_t dino;
    int floorHeight;
    uint64_t time;
}world_t;

void spawnObstacles(world_t* world){
    world->obstacles[0].pos = 10;
    for(int i = 1; i < world->obstaclesCount; i++){
        world->obstacles[i].pos = (rand() % 9) + world->obstacles[i-1].pos + 5;
    }
    world->farthestObstacleIndex = world->obstaclesCount - 1;
}

void drawFloor(WINDOW* screen, int floorHeight){
    int xMax = getmaxx(screen);
    int yMax = getmaxy(screen);
    for (int i = yMax - floorHeight; i < yMax + 1; i++){
        for (int j = 0; j < xMax; j++){
            mvwaddch(screen, i, j, '#');
        }
    }
}

void drawDino(WINDOW* screen, world_t* world){
    int yMax = getmaxy(screen);
    mvwaddch(screen, yMax - world->floorHeight - 2 - world->dino.distanceToGround, world->dino.position, '@');
    mvwaddch(screen, yMax - world->floorHeight - 1 - world->dino.distanceToGround, world->dino.position, '@');
}

void drawObstacles(WINDOW* screen, world_t* world){
    int yTarget = getmaxy(screen) - world->floorHeight - 1;
    for (int i = 0; i < world->obstaclesCount; i++){
        mvwaddch(screen, yTarget, world->obstacles[i].pos, '*');
    }
}

int draw(WINDOW* screen, world_t* world){
    werase(screen);
    drawFloor(screen, world->floorHeight);
    drawDino(screen, world);
    drawObstacles(screen, world);
    wrefresh(screen);
    return 0;
}

int process(world_t* world){
    if (world->time % world->dino.jumpTime == 0){
        if (world->dino.isJumping == 1){
            world->dino.distanceToGround++;
            if (world->dino.distanceToGround == world->dino.jumpHeight){
                world->dino.isJumping = 0;
            }
        }
        else {
            world->dino.distanceToGround = MAX(world->dino.distanceToGround - 1, 0);
        }
    }

    // process obstacles
    int moveTime = world->time % world->obstaclesMoveTime;
    for (int i = 0; i < world->obstaclesCount; i++){
        //If it killed dino
        if (world->obstacles[i].pos == world->dino.position && world->dino.distanceToGround == 0){
            return 1;
        }

        // If I need to respawn it
        if (world->obstacles[i].pos == 0){
            world->obstacles[i].pos = (rand() % 9) + world->obstacles[world->farthestObstacleIndex].pos + 5;
            world->farthestObstacleIndex = i;
        }

        // If it needs to be moved
        if (moveTime == 0){
            world->obstacles[i].pos--;
        }
    }

    world->time++;
    return 0;
}

int handleInput(world_t* world) {
    char input = getch();
    switch (input) {
        case 'w':
            if (world->dino.distanceToGround == 0) {
                world->dino.isJumping = 1;
            }
            break;

        case 'q':
            return 1;

        default:
            break;
    }
    return 0;
}

int main() {
    srand(time(NULL));

    setUpWindow();

    world_t world = {.obstacles = malloc(sizeof(obstacle_t) * 10), .obstaclesCount = 10, .obstaclesMoveTime = 13,
                     .dino = {.distanceToGround = 0, .jumpHeight = 3, .jumpTime = 9, .isJumping = 0, .position = 2},
                     .floorHeight = 10};
    spawnObstacles(&world);

    int state = 0;
    while (state == 0){
        state = 0;
        state |= process(&world);
        state |= handleInput(&world);
        state |= draw(stdscr, &world);
        usleep(16 * 1000);
    }

    endwin();
    return 0;
}
