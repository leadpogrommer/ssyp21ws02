#include "popup.h"
#include <stdlib.h>
#include <string.h>

popup_t* init_popup(WINDOW* main_window, int speed, char position, const char* format_string, ...){
    popup_t* popup = calloc(sizeof(popup_t), 1);
    popup->speed = speed;

    va_list args;
    va_start(args, format_string);
    char message[256];
    vsprintf(message, format_string, args);
    va_end(args);

    int height, width, current_width, i;
    width = current_width = i = 0;
    height = 1;
    while (message[i++]){
        current_width++;
        width = current_width > width ? current_width : width;
        if (message[i] == '\n'){
            height++;
            current_width = 0;
        }
    }

    char* corrected_message = calloc(sizeof(char), width * height);
    current_width = 0;
    int current_offset = 0;
    for (i = 0; i < (int)strlen(message); i++){
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

    vector2_t popup_position = VEC2_ZERO;

    if (position == POPUP_URCORNER){
        popup_position.x = getmaxx(main_window) - width;
    }

    popup->window = newwin(height, width, popup_position.y, popup_position.x);
    popup->move_fraction = 250;
    popup->live_time = 1000;

    return popup;
}

void destroy_popup(popup_t* popup){
    free(popup->message);
    free(popup);
}

void process_popup(popup_t** popup_ref){
    popup_t* popup = *popup_ref;
    popup->progress += popup->speed;
    if (popup->progress >= popup->live_time){
        destroy_popup(*popup_ref);
        *popup_ref = NULL;
        return;
    }
    *popup_ref = popup;
}
