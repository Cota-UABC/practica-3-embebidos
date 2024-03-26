#ifndef UART_H
#define UART_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include <string.h>

#define STR_SIZE 100
#define TX_PIN (GPIO_NUM_17)
#define RX_PIN (GPIO_NUM_16)
#define RING_BUFFER_SIZE 1024
#define BUFFER_SIZE 1024*2
#define TASK_MEMORY 1024*6
#define SYNC_CODE "50567856B"

//UART 1 macros
#define TRANSFER_STRING(str_literal, str) sprintf((str), str_literal); UART_transfer(UART_1,(str), 0);
#define SKIP_LINE(num,str) sprintf((str), "\033[%dE",(num)); UART_transfer(UART_1,(str), 0);
#define CLEAR_SCREEN(str) sprintf((str), "\033[2J"); UART_transfer(UART_1,(str), 0);
#define HOME_POS(str) sprintf((str), "\033[2J\033[H"); UART_transfer(UART_1,(str), 0);
#define SAVE_POS(str) sprintf((str), "\0337"); UART_transfer(UART_1,(str), 0);
#define RESTORE_POS(str) sprintf((str), "\0338"); UART_transfer(UART_1,(str), 0);
#define COLOR_GREEN(str) sprintf((str), "\033[32m"); UART_transfer(UART_1,(str), 0);
#define COLOR_RED(str) sprintf((str), "\033[31m"); UART_transfer(UART_1,(str), 0);
#define COLOR_DEFAULT(str) sprintf((str), "\033[39m"); UART_transfer(UART_1,(str), 0);

extern const uart_port_t UART_1, UART_2;
extern QueueHandle_t uart_queue;
extern const char *tag, *sync_code;
extern volatile uint8_t u1_rx_buff_data[BUFFER_SIZE], u2_rx_buff_data[BUFFER_SIZE];
extern volatile int  u1_rx_buff_data_index, u2_rx_buff_data_index;
extern uint8_t rx1_f, enter_f, echo_f, sync_f;
extern int cursor_pos;

void init_UART(void);
void UART_transfer(const uart_port_t uart_n,char* str, int len);
void UART_transfer_char(const uart_port_t uart_n,char ch);
void UART_receive();
esp_err_t create_uart_tasks(void);
void uart1_rx_task(void *pvParameters);
void uart2_rx_task(void *pvParameters);
void clear_buffer(volatile uint8_t *buffer, volatile int *index);

#endif