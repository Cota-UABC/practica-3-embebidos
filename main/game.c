#include "game.h"

const char *tag_g = "GAME";
eGameState_t game_state = main_menu;
static char str[STR_SIZE];

void mainGame(void)
{

    switch(game_state)
    {
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

void mainMenu(void)
{
    //ESP_LOGI(tag_g,"Game started");
    static int reset_flag = 1, press_e = 0;
    echo_f = 1;//activate echo

    if(reset_flag == 1)
    {
        //send sync code to erease buffer of the other player
        UART_transfer(UART_2, sync_code,0);

        rx1_f = 1; //activate rx buffer process
        cursor_pos = 0;//reset pos
        enter_f = 0;//reset if enter key was pressed
        strcpy(str, "\033[2J\033[HEscoge rol a usar:\033[1ERetador[R]  Jugador[J]\033[1EIngresa la letra y presiona Enter: ");
        UART_transfer(UART_1,str, 0);
        reset_flag = 0;

        //clear rx2 array
        clear_buffer(u2_rx_buff_data, &u2_rx_buff_data_index);
    }
    else if(press_e && u1_rx_buff_data_index)//opcion invalida, esperar teclaso
    {
        press_e=0;
        reset_flag=1;//reset state
        //clear rx array
        clear_buffer(u1_rx_buff_data, &u1_rx_buff_data_index);
    }
    else if(enter_f)
    {
        enter_f=0;//clear enter key flag
        if(u1_rx_buff_data[0]== 'r' || u1_rx_buff_data[0]== 'R' )
        {
            strcpy(str, "\033[2J\033[HHas escogido RETADOR");
            UART_transfer(UART_1, str, 0);

            //send to player
            UART_transfer_char(UART_2, u1_rx_buff_data[0]);
            //change game state
            game_state++;
        }
        else if(u1_rx_buff_data[0]== 'j' || u1_rx_buff_data[0]== 'J' )
        {
            strcpy(str, "\033[2J\033[HHas escogido JUGADOR");
            UART_transfer(UART_1, str, 0);

            //send to player
            UART_transfer_char(UART_2, u1_rx_buff_data[0]);
            //change game state
            game_state++;
        }
        else 
        {
            strcpy(str, "\033[2J\033[HOpcion no valida, presiona cualquier tecla.");
            UART_transfer(UART_1, str, 0);
            press_e = 1;//reset state
        }
        //clear rx array
        clear_buffer(u1_rx_buff_data, &u1_rx_buff_data_index);
    }

}

void checkChoices(void)
{
    static int reset_flag = 1;

    if(reset_flag == 1)
    {
        enter_f=0;//clear enter key flag
        rx1_f=0;

        strcpy(str, "\033[1E\033[1EEsperando al otro jugador...");
        UART_transfer(UART_1,str, 0);

        reset_flag = 0;
    }
    //data received
    else if(u2_rx_buff_data_index != 0)
    {
        strcpy(str, "\033[1ELa respuesta obtenida fue: ");
        UART_transfer(UART_1,str, 0);
        strcpy(str, (const char*)u2_rx_buff_data);
        UART_transfer(UART_1,str, 0);

        //clear rx array
        clear_buffer(u2_rx_buff_data, &u2_rx_buff_data_index);
    }
}