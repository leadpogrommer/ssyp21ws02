#include "popup.h"
#include <stdlib.h>

popup_t* init_popup(char* message, int speed){
    popup_t* popup = calloc(sizeof(popup_t), 1);
    popup->speed = speed;

    int height, width, current_width, i;
    width = current_width = i = 0;
    height = 1;
    while (message[i]){
        current_width++;
        width = current_width > width ? current_width : width;
        if (message[i] == '\n'){
            height++;
            current_width = 0;
        }
        i++;
    }

    char* corrected_message = calloc(sizeof(char), width * height);
    current_width = 0;
    int current_offset = 0;
    for (i = 0; i < width * height; i++){
        if (message[i] == '\n'){
            for (int j = 0; j < width - current_width; j++){
                corrected_message[i + j + current_offset] = ' ';
            }
            current_offset += width - current_width;
            current_width = 0;
            corrected_message[i + current_offset++] = '\n';
            corrected_message[i + current_offset] = ' ';
        }else{
            corrected_message[i + current_offset] = message[i];
            current_width++;
        }

    }
    popup->message = corrected_message;

    popup->message_size = (vector2_t){ width, height };
    width += 2;
    height += 2;
    popup->window = newwin(height, width, 0, 0);
    popup->move_fraction = 30;

    return popup;
}

void destroy_popup(popup_t* popup){
    free(popup->message);
    free(popup);
}

void process_popup(popup_t** popup_ref){
    popup_t* popup = *popup_ref;
    popup->progress += popup->speed;
    if (popup->progress >= 100){
        destroy_popup(popup);
        *popup_ref = NULL;
        return;
    }
    *popup_ref = popup;
}