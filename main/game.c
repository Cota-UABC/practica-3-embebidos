#include "game.h"

const char *tag_g = "GAME";
char secret_word[STR_SIZE];
uint8_t correct_f, incorrect_f, word_count, incrt_count;
eGameState_t game_state = 0;
eMainMenuState_t main_state;
eCheckChoicesState_t choices_state;
eSecretWordState_t secret_word_state;
eWaitKeyExit_t wait_key_exit;
ePlayingState_t playing_state;
eAgainState_t again_state;
ePlayerChoice_t player1_choice, player2_choice;
static char str[STR_SIZE];

void mainGame(void)
{
    if(sync_f>1) //reset game if synced more than once
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
        case secret_word_g:
            secretWord();
            break;
        case playing_g:
            playing();
            break;
        case again:
            again();
            break;
        default:
            HOME_POS(str)
            COLOR_RED(str)
            TRANSFER_STRING("ERROR DEFAULT STATE", str)
            break;
    }
}

void resetGame(void)
{
    sync_f=0;
    main_state=0;//reset all but game state enums
    choices_state=0;
    secret_word_state=0;
    playing_state=0;
    game_state++;
    bzero((uint8_t *)secret_word,STR_SIZE);
    //send sync code so other player clears its buffer
    UART_transfer(UART_2, (char*)sync_code,0);
}

void mainMenu(void)
{
    //ESP_LOGI(tag_g,"Game started");
    echo_f = 1;//activate echo

    switch(main_state)
    {
        case reset_mm:
            HOME_POS(str);
            TRANSFER_STRING("Escoge rol a usar:\033[1ERetador[R]  Jugador[J]\033[1EIngresa la letra y presiona Enter: ", str)

            activateInput();

            //clear rx2 array
            clear_buffer(u2_rx_buff_data, &u2_rx_buff_data_index);
            main_state++;
            break;
        case waiting_enter_mm:
            if(enter_f)
            {
                enter_f=0;//clear enter key flag
                if(u1_rx_buff_data[0]== 'r' || u1_rx_buff_data[0]== 'R' || u1_rx_buff_data[0]== 'j' || u1_rx_buff_data[0]== 'J')
                {
                    NO_INPUT_PROCESS
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
                    TRANSFER_STRING("\033[2J\033[HOpcion no valida, presiona cualquier tecla.",str)
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
            NO_INPUT_PROCESS
            TRANSFER_STRING("\033[2EEsperando al otro jugador...", str)
            choices_state++;
            break;
        case waiting_data:
            if(u2_rx_buff_data_index != 0) //data received
            {
                TRANSFER_STRING("\033[1EEl otro jugador eligio ", str)
                if(u2_rx_buff_data[0]=='r' || u2_rx_buff_data[0]=='R')
                {   
                    TRANSFER_STRING("retador.", str)
                    player2_choice=retador;
                    choices_state++;
                }
                else if(u2_rx_buff_data[0]=='j' || u2_rx_buff_data[0]=='J')
                {   
                    TRANSFER_STRING("jugador.", str)
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
                //clear rx2 array
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
                NO_INPUT_PROCESS
                switch (wait_key_exit)
                {
                    case reset_current_state:   
                        choices_state=0;
                        break;
                    case next_state:
                        game_state++;
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
    switch(secret_word_state)
    {
        case reset_sw:
            if(player1_choice==retador)
            {
                HOME_POS(str)
                TRANSFER_STRING("Ingresa la palabra secreta y presione Enter\033[1E(ESPACIOS Y MAYUSCULAS SERAN IGNORADOS): \033[1E->", str)
                activateInput();
                secret_word_state++;
            }
            else if(player1_choice==jugador)
            {
                HOME_POS(str)
                TRANSFER_STRING("Esperando palabra secreta...", str)
                secret_word_state=wait_secret_word;
            }
            break;
        case wait_enter_sw:
            if(enter_f && u1_rx_buff_data[0] != 13)
            {
                NO_INPUT_PROCESS
                for(int i=0;i<u1_rx_buff_data_index;i++)
                {
                    if(u1_rx_buff_data[i] == 13 || u1_rx_buff_data[i] == 32)//break if enter or space found
                        break;
                    if(u1_rx_buff_data[i] >= 65 && u1_rx_buff_data[i] <= 90) //convert to lowercase
                        u1_rx_buff_data[i]+=32;
                    UART_transfer_char(UART_2,u1_rx_buff_data[i]); 
                    secret_word[i] = (char)u1_rx_buff_data[i];
                }
                //ESP_LOGI(tag_g, "secret word: %s",secret_word);
                UART_transfer_char(UART_2,27); //esc terminator
                SKIP_LINE(2,str)
                TRANSFER_STRING("Esperando respuesta del jugador..", str)
                activateInput();
                secret_word_state=wait_j_responce;
            }
            else if(u1_rx_buff_data[0] == 13)
            {
                SAVE_POS(str)
                COLOR_RED(str)
                TRANSFER_STRING("\033[2EINGERESE AL MENOS UNA LETRA", str)
                COLOR_DEFAULT(str)
                RESTORE_POS(str)
                enter_f=0; //reset enter flag
                clear_buffer(u1_rx_buff_data, &u1_rx_buff_data_index);//clear rx1 array
            }
            break;
        case wait_secret_word:
            if(u2_rx_buff_data_index != 0) // if received data
            if(u2_rx_buff_data[u2_rx_buff_data_index-1] == 27)
            {
                for(int i=0;i<u2_rx_buff_data_index-1;i++)//copy secret word
                {
                    if(i>=STR_SIZE) //if end of string
                        break;
                    secret_word[i] = (char)u2_rx_buff_data[i]; 
                }
                //ESP_LOGI(tag_g, "secret word: %s",secret_word);

                SKIP_LINE(2,str)
                COLOR_GREEN(str)
                TRANSFER_STRING("Palabra secreta recibida", str)
                COLOR_DEFAULT(str)
                clear_buffer(u2_rx_buff_data, &u2_rx_buff_data_index);//clear rx2 array

                SKIP_LINE(2,str)
                TRANSFER_STRING("Presiona Enter para continuar..", str)
                activateInput();
                secret_word_state=wait_key_sw;
            }
            break;
        case wait_key_sw:
            if(u1_rx_buff_data_index>0)
            {
                UART_transfer_char(UART_2,27); //esc para que el retador continue
                game_state++;
            }
            break;
        case wait_j_responce:
            if(u2_rx_buff_data_index != 0) // if received data
            if(u2_rx_buff_data[u2_rx_buff_data_index-1] == 27)
            {
                game_state++;
                clear_buffer(u2_rx_buff_data, &u2_rx_buff_data_index);//clear rx2 array
            }
            break;
    }
}

void playing(void)
{
    switch(playing_state)
    {
        case reset_p:
            NO_INPUT_PROCESS 
            word_count=0;
            incrt_count=0;
            correct_f=0;
            incorrect_f=0;
            HOME_POS(str)
            TRANSFER_STRING("   +---+\033[1E   |    |\033[1B\033[1D|\033[1B\033[1D|\033[1B\033[1D|\033[1B\033[1D|", str)
            SKIP_LINE(3,str)
            for(int i=0;i<strlen(secret_word);i++)
            {
                TRANSFER_STRING("_ ", str)
            }

            if(player1_choice == retador)
            {
                playing_state=update_p;
                HIDE_CURSOR(str)
            }

            else if(player1_choice == jugador)
            {
                SKIP_LINE(2,str)
                TRANSFER_STRING("Ingrese letra y presione Enter: ", str)
                SAVE_POS(str)
                activateInput();
                playing_state=wait_enter_p;
            }
            break;
        case update_p:
            //ajustar banderas del retador en base a respuesta del jugador
            if(player1_choice == retador)
            {
                if(u2_rx_buff_data_index != 0)// if received data
                { 
                    if(u2_rx_buff_data[0] == '0')
                    {
                        incorrect_f=1;
                    }
                    else if(u2_rx_buff_data[0] == '1')
                    {
                        correct_f=1;
                    }
                    clear_buffer(u2_rx_buff_data, &u2_rx_buff_data_index);//clear rx2 array
                }
            }

            //actualizar juego en base a banderas
            if(correct_f)
            {
                //ESP_LOGI(tag_g, "nums: %d ",word_count);
                MOVE_TO_POS(9, word_count*2+1, str)
                UART_transfer_char(UART_1,secret_word[word_count]);
                word_count++;
                correct_f=0;
            }
            else if(incorrect_f)
            {
                updateHangman(incrt_count);
                incorrect_f=0;
                incrt_count++;
            }

            //activar entrada de datos para jugador
            if(player1_choice == jugador)
            {
                playing_state=wait_enter_p;
                activateInput();
                RESTORE_POS(str)
                SAVE_POS(str)
            }

            //terminar juego
            //ESP_LOGI(tag_g, "nums: %d %d",word_count, strlen(secret_word));
            if(incrt_count == 6 || word_count == strlen(secret_word))
                playing_state=game_over;
            break;
        case wait_enter_p:
            if(enter_f)
            {
                NO_INPUT_PROCESS 
                RESTORE_POS(str)
                SAVE_POS(str)
                CLEAR_LINE(str)
                //ESP_LOGI(tag_g, "Chars: %s %c %d %c",secret_word,secret_word[word_count],word_count,u1_rx_buff_data[0]);
                if(secret_word[word_count] == u1_rx_buff_data[0])
                {
                    correct_f=1;
                    UART_transfer_char(UART_2,'1');
                }
                else 
                {
                    incorrect_f=1;
                    UART_transfer_char(UART_2,'0');
                }
                playing_state=update_p;
            }
            break;
        case game_over:
            NO_INPUT_PROCESS
            SHOW_CURSOR(str)
            MOVE_TO_POS(13, 1, str)
            if(incrt_count == 6)
            {
                COLOR_RED(str)
                if(player1_choice == jugador)
                {   
                    TRANSFER_STRING("PERDISTE EL JUEGO. La palabra era: ", str)
                    UART_transfer(UART_1,secret_word, 0);
                    TRANSFER_STRING("\033[1EPresiona Enter para continuar...", str)
                }
                else 
                {   
                    TRANSFER_STRING("JUGADOR PERDIO EL JUEGO. La palabra era: ", str)
                    UART_transfer(UART_1,secret_word, 0);
                    TRANSFER_STRING("\033[1EPresiona Enter para continuar...", str)
                }
                COLOR_DEFAULT(str)
            }
            else 
            {
                COLOR_GREEN(str)
                TRANSFER_STRING("!!JUEGO GANADO!! Presiona Enter para continuar...", str)
            }
            playing_state++;
            activateInput();
            break;
        case wait_key_p:
            if(u1_rx_buff_data_index)
            {
                NO_INPUT_PROCESS
                game_state++;
            }
            break;
    }
}

void again(void)
{
    //
}

void activateInput(void)
{
    rx1_f = 1; //activate rx buffer process
    cursor_pos = 0;//reset pos
    enter_f = 0;//reset if enter key was pressed
    clear_buffer(u1_rx_buff_data, &u1_rx_buff_data_index);//clear rx1 array
}

void updateHangman(uint8_t incrt_count)
{
    switch(incrt_count)
    {
        case 0:
            MOVE_TO_POS(3, 4, str)
            UART_transfer_char(UART_1,'0');
            break;
        case 1:
            MOVE_TO_POS(4, 4, str)
            UART_transfer_char(UART_1,'|');
            break;
        case 2:
            MOVE_TO_POS(4, 3, str)
            UART_transfer_char(UART_1,'/');
            break;
        case 3:
            MOVE_TO_POS(4, 5, str)
            UART_transfer_char(UART_1,'\\');
            break;
        case 4:
            MOVE_TO_POS(5, 3, str)
            UART_transfer_char(UART_1,'/');
            break;
        case 5:
            MOVE_TO_POS(5, 5, str)
            UART_transfer_char(UART_1,'\\');
            break;
    }
}