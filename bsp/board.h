#ifndef BOARD_H
#define BOARD_H

/* LEDs, buttons, speaker, mic and serial port */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "gpio.h"
#include "serial.h"
#include "pwm.h"
#include "adc.h"
#include "audio.h"
#include "timer.h"

/* LEDs definitions for 5x5 matrix display */
#define LED_NUM_ROWS    5
#define LED_NUM_COLS    5

#define LED_ROW1    21
#define LED_ROW2    22
#define LED_ROW3    15
#define LED_ROW4    24
#define LED_ROW5    19

#define LED_COL1    28
#define LED_COL2    11
#define LED_COL3    31
#define LED_COL4    37
#define LED_COL5    30

/* Buttons */
#define BUTTONS_NUMBER 2

#define BUTTON_0    14
#define BUTTON_1    23

#define BUTTONS_ACTIVE_STATE 0

/* UART */
#define UART_RX     40   // P1_08
#define UART_TX     06   // P0_06

/* On-board speaker and mic */
#define SPEAKER     0   // P0_00
#define MIC         3   // AIN3 on P0_05
                        //  (we need to specify analog input channel,
                        //   and not the pin number)
#if 1   // disable mic
#define RUN_MIC     0
#else
#define RUN_MIC     20  // P0_20
#endif

/* Function prototypes */

/* board */
void board_init(void);

/* LED */
void leds_init(void);
void led_display(uint8_t pic[LED_NUM_ROWS][LED_NUM_COLS]);

/* Turn on/off, glow or blink a specific LED for debugging. */
void led_on(int row, int col);
void led_off(int row, int col);
#define BLINK_FOREVER -1
void led_glow(int row, int col);
void led_blink(int row, int col, int count);

/* Buttons */
void buttons_init(void);
uint32_t button_get(uint32_t button_idx);

/* Delay */
void delay_us(uint32_t us);
void delay_ms(uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif // BOARD_H
