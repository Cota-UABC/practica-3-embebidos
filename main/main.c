#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include <string.h>
#include "uart.h"
#include "game.h"


void intToChar(int number, char* result);
void delayMs(uint32_t ms);

void intToChar(int num, char* result) {
    sprintf(result, "%d", num);
}

void delayMs(uint32_t ms) {
    vTaskDelay(ms / portTICK_PERIOD_MS);
}

void app_main(void)
{
    init_UART(); 
    create_uart_tasks();

    //strcpy(str, "test ");
    //int length = 0;

    while(1)
    {

        mainGame();

        delayMs(500);
    }
}