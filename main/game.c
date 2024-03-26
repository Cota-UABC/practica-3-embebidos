#include "game.h"

const char *tag_g = "GAME";
eGameState_t game_state = 0;
eMainMenuState_t main_state;
eCheckChoicesState_t choices_state;
eWaitKeyExit_t wait_key_exit;
ePlayerChoice_t player1_choice, player2_choice;
static char str[STR_SIZE];

void mainGame(void)
{
    if(sync_f>1)
        game_state = 0;
    switch(game_state)
    {
        case reset_game:
            resetGame();
            break;
        case main_menu:
            mainMenu();
            break;
        case check_choices:
            checkChoices();
            break;
        default:
            break;
    }
}

void resetGame(void)//reset all but game state enums
{
    sync_f=0;
    main_state=0;
    choices_state=0;
    game_state++;
    //send sync code so other player clears its buffer
    UART_transfer(UART_2, sync_code,0);
}

void mainMenu(void)
{
    //ESP_LOGI(tag_g,"Game started");
    echo_f = 1;//activate echo

    switch(main_state)
    {
        case reset_mm:
            rx1_f = 1; //activate rx buffer process
            cursor_pos = 0;//reset pos
            enter_f = 0;//reset if enter key was pressed

            CLEAR_SCREEN(str);
            HOME_POS(str);
            TRANSFER_STRING("Escoge rol a usar:\033[1ERetador[R]  Jugador[J]\033[1EIngresa la letra y presiona Enter: ", str)

            //clear rx2 array
            clear_buffer(u2_rx_buff_data, &u2_rx_buff_data_index);
            //clear rx1 array
            clear_buffer(u1_rx_buff_data, &u1_rx_buff_data_index);
            main_state++;
            break;
        case waiting_enter:
            if(enter_f)
            {
                enter_f=0;//clear enter key flag
                if(u1_rx_buff_data[0]== 'r' || u1_rx_buff_data[0]== 'R' || u1_rx_buff_data[0]== 'j' || u1_rx_buff_data[0]== 'J')
                {
                    rx1_f=0; //deactivate rx buffer process
                    TRANSFER_STRING("\033[2J\033[HHas escogido: ", str)
                    if(u1_rx_buff_data[0]== 'r' || u1_rx_buff_data[0]== 'R')
                    {   TRANSFER_STRING("RETADOR", str) player1_choice=retador;}
                    if(u1_rx_buff_data[0]== 'J' || u1_rx_buff_data[0]== 'j')
                    {   TRANSFER_STRING("JUGADOR", str) player1_choice=jugador;}

                    //send to player
                    UART_transfer_char(UART_2, u1_rx_buff_data[0]);
                    //change game state
                    game_state++;
                }
                else 
                {
                    strcpy(str, "\033[2J\033[HOpcion no valida, presiona cualquier tecla.");
                    UART_transfer(UART_1, str, 0);
                    main_state++;
                }
                //clear rx array
                clear_buffer(u1_rx_buff_data, &u1_rx_buff_data_index);
            }
            break;
        case wait_key_mm:
            if(u1_rx_buff_data_index)
                main_state=0;
            break;
    }
}

void checkChoices(void)
{
    switch(choices_state)
    {
        case reset_cc:
            rx1_f=0; //deactivate rx buffer process
            TRANSFER_STRING("\033[2EEsperando al otro jugador...", str)
            choices_state++;
            break;
        case waiting_data:
            if(u2_rx_buff_data_index != 0) //data received
            {
                TRANSFER_STRING("\033[1ELa respuesta obtenida fue: ", str)
                if(u2_rx_buff_data[0]=='r' || u2_rx_buff_data[0]=='R')
                {   
                    TRANSFER_STRING("RETATDOR", str)
                    player2_choice=retador;
                    choices_state++;
                }
                else if(u2_rx_buff_data[0]=='j' || u2_rx_buff_data[0]=='J')
                {   
                    TRANSFER_STRING("JUGADOR", str)
                    player2_choice=jugador;
                    choices_state++;
                }
                else
                {
                    TRANSFER_STRING("Opcion invalida. Presiona cualquier tecla..", str)
                    choices_state=wait_key_cc;
                    wait_key_exit=reset_current_state;
                    rx1_f=1; //activate rx buffer process
                    clear_buffer(u1_rx_buff_data, &u1_rx_buff_data_index);
                }
                //clear rx array
                clear_buffer(u2_rx_buff_data, &u2_rx_buff_data_index);
            }
            break;
        case compare_choices:
            if(player1_choice != player2_choice)
            {
                COLOR_GREEN(str)
                TRANSFER_STRING("\033[2EAmbos eligieron distintos roles. Presiona una tecla para continuar...", str)
                COLOR_DEFAULT(str)
                wait_key_exit=next_state;
            }
            else if(player1_choice == player2_choice)
            {
                COLOR_RED(str)
                TRANSFER_STRING("\033[2EAmbos eligieron roles IGUALES. Presiona una tecla para reiniciar...", str)
                COLOR_DEFAULT(str)
                wait_key_exit=reset_g;
            }
            rx1_f=1; //activate rx buffer process
            clear_buffer(u1_rx_buff_data, &u1_rx_buff_data_index);
            choices_state=wait_key_cc;
            break;
        case wait_key_cc:
            if(u1_rx_buff_data_index)
            {
                rx1_f=0; //deactivate rx buffer process
                switch (wait_key_exit)
                {
                    case reset_current_state:   
                        choices_state=0;
                        break;
                    case next_state:
                        //game_state++;
                        break;
                    case reset_g:
                        game_state=0;
                        break;
                }
            }
            break;   
    }
}

void secretWord(void)
{
    //
}