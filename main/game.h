#ifndef GAME_H
#define GAME_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <string.h>
#include "uart.h"

#define STR_SIZE 100

typedef enum gm_state {
    main_menu,
    check_choices,
    secret_word,
    playing,
    again,
    end
}eGameState_t;

extern eGameState_t game_state;

void mainGame(void);
void mainMenu(void);
void checkChoices(void);

#endif