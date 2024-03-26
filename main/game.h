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
    reset_game,
    main_menu,
    check_choices,
    secret_word,
    playing,
    again,
    end
}eGameState_t;

typedef enum mm_state {
    reset_mm,
    waiting_enter,
    wait_key_mm
}eMainMenuState_t;

typedef enum cc_state {
    reset_cc,
    waiting_data,
    compare_choices,
    wait_key_cc
}eCheckChoicesState_t;

typedef enum wait_exit {
    reset_current_state,
    next_state,
    reset_g
}eWaitKeyExit_t;

extern const char *tag_g;
extern eGameState_t game_state;
extern eMainMenuState_t main_state;
extern eCheckChoicesState_t choices_state;
extern eWaitKeyExit_t wait_key_exit;

typedef enum player_choice {
    retador,
    jugador
}ePlayerChoice_t;

extern ePlayerChoice_t player1_choice, player2_choice;

void mainGame(void);
void resetGame(void);
void mainMenu(void);
void checkChoices(void);
void secretWord(void);

#endif