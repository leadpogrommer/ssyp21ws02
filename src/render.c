#include "render.h"
#include <stdlib.h>

palette_t* init_palette(){
    palette_t* palette = malloc(sizeof(palette_t));
    for (int i = 0; i < 256; i++){
        palette->symbol[i] = i;
    }
}

void destroy_palette(palette_t* palette){
    free(palette);
}