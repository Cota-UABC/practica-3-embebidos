#include <stdio.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>
#include <strings.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"

#define DONT_WAIT 0 
#define SINGLE_BYTE 1
#define QUEUE_SIZE 10
#define BUF_SIZE (1024)
#define FIRST_PRINTABLE_CHAR 32
#define LAST_PRINTABLE_CHAR 126
#define NULL_CHAR 0
#define BACKSPACE 8

static QueueHandle_t uart0_queue;
static uart_event_t uart_event;

void delayMs(uint32_t ms) {
    vTaskDelay(ms / portTICK_PERIOD_MS);
}

bool UART_kbhit(void) {
  uint8_t length;
  uart_get_buffered_data_len(UART_NUM_0, (size_t*)&length);
  return (length > 0);
}

char UART_getchar(void) {
  uint8_t c;
  while(!UART_kbhit()){
    delayMs(1);
  }
  uart_read_bytes(UART_NUM_0, &c, SINGLE_BYTE, DONT_WAIT);
  return c;
}

void UART_putchar(char c) {
  uart_write_bytes(UART_NUM_0, &c, SINGLE_BYTE);
}

void UART_puts(char *str) {
  while(*str) UART_putchar(*str++);
}

void UART_gets(char *str) {
  char c;
  uint32_t i = 0;
  while((c = UART_getchar()) != '\r'){
    if(c >= FIRST_PRINTABLE_CHAR && c <= LAST_PRINTABLE_CHAR){
      str[i++] = c;
    }
    if(c == BACKSPACE && i == 0)
      continue;
    else if(c == BACKSPACE){
      if(i-- > 0){
        continue;
      }
    }
  }
  str[i] = 0;
}

void uartInit() {
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    uart_driver_install(UART_NUM_0, BUF_SIZE, BUF_SIZE, QUEUE_SIZE, &uart0_queue, 0);
    uart_param_config(UART_NUM_0, &uart_config);
    uart_set_pin(UART_NUM_0, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

// void UART_task(){
//   char buffer[BUF_SIZE];
//   bzero(buffer, BUF_SIZE);
//   UART_putchar('\n');
//   while(true){
//     UART_gets(buffer);
//     UART_puts(buffer);
//   }
// }

void app_main(void) {
  uartInit();
  // xTaskCreate(UART_task,"uart_task", BUF_SIZE * 2, NULL, 10, NULL);
  char buffer[BUF_SIZE];
  bzero(buffer, BUF_SIZE);
  UART_putchar('\n');

    char str[] = "hello";

  while(true){
    //UART_gets(buffer);
    UART_puts(str);
    delayMs(500);
  }
}