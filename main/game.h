#ifndef GAME_H
#define GAME_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "uart.h"

#define STR_SIZE 100

typedef enum gm_state {
    reset_game,
    main_menu,
    check_choices,
    secret_word_g,
    playing_g,
    again,
    end
}eGameState_t;

//game state states
typedef enum mm_state {
    reset_mm,
    waiting_enter_mm,
    wait_key_mm
}eMainMenuState_t;

typedef enum cc_state {
    reset_cc,
    waiting_data,
    compare_choices,
    wait_key_cc
}eCheckChoicesState_t;

typedef enum sw_state {
    reset_sw,
    wait_enter_sw,
    wait_secret_word,
    wait_key_sw,
    wait_j_responce
}eSecretWordState_t;

typedef enum p_state {
    reset_p,
    update_p,
    wait_enter_p,
    game_over,
    wait_key_p
}ePlayingState_t;

typedef enum a_state {
    reset_p,
    update
}eAgainState_t;

typedef enum wait_exit {
    reset_current_state,
    next_state,
    reset_g
}eWaitKeyExit_t;

extern const char *tag_g;
extern char secret_word[STR_SIZE];
extern uint8_t correct_f, incorrect_f, word_count, incrt_count;
extern eGameState_t game_state;
extern eMainMenuState_t main_state;
extern eCheckChoicesState_t choices_state;
extern eSecretWordState_t secret_word_state;
extern ePlayingState_t playing_state;
extern eAgainState_t again_state;
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
void playing(void);
void again(void);
void activateInput(void);
void updateHangman(uint8_t incrt_count);

#endif