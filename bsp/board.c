#include <stdint.h>
#include "board.h"
#include "lib.h"

/* LEDs, buttons, speaker and mic routines */

const uint8_t led_rows[] =
    { LED_ROW1, LED_ROW2, LED_ROW3, LED_ROW4, LED_ROW5 };
const uint8_t led_cols[] =
    { LED_COL1, LED_COL2, LED_COL3, LED_COL4, LED_COL5 };
static const uint8_t btn_list[] = { BUTTON_0, BUTTON_1 };

void board_init(void)
{
    leds_init();
    buttons_init();
    serial_init(UART_RX, UART_TX);
    //audio_init(SPEAKER, MIC, RUN_MIC);
    /*timer_init();*/
}

/* Initialize LED matrix.
 *
 * Start with a blank display.
 * Rows are held low and columns are held high to reverse bias all the LEDs.
 */
void leds_init(void)
{
    uint32_t r, c;

    /* Rows */
    for (r = 0; r < LED_NUM_ROWS; r++)
    {
        gpio_clear(led_rows[r]);                // row lines low
        gpio_out(led_rows[r], DRIVE_STANDARD);  // standard drive current
    }

    /* Columns */
    for (c = 0; c < LED_NUM_COLS; c++)
    {
        gpio_set(led_cols[c]);                  // column lines high
        gpio_out(led_cols[c], DRIVE_STANDARD);  // standard drive current
    }
}

/* Display the picture on the LED matrix. */
void led_display(uint8_t pic[LED_NUM_ROWS][LED_NUM_COLS])
{
    /* Refresh columns one-by-one. */
    for (int c = 0; c < LED_NUM_COLS; c++)
    {
        /* Activate current column by pulling the column signal low. */
        gpio_clear(led_cols[c]);

        /* Blink the row LEDs one-by-one. */
        for (int r = 0; r < LED_NUM_ROWS; r++)
        {
            /* If a pixel is set, turn on for a while and then turn off */
            if (pic[r][c])
                gpio_set(led_rows[r]);      // turn on
            else
                gpio_clear(led_rows[r]);    // turn off

            /* Keep the LED on for a while. For NxN display, 1 ms delay
             * amounts to approximately 1000/NxN frames per second.
             */
            delay_ms(1);

            gpio_clear(led_rows[r]);
        }

        /* Dectivate current column by pulling the column signal high. */
        gpio_set(led_cols[c]);
    }
}

/* Configure buttons pins */
void buttons_init(void)
{
    uint32_t i;

    for (i = 0; i < BUTTONS_NUMBER; ++i)
    {
        gpio_in(btn_list[i], PULLUP);
    }
}

/* Get button state */
uint32_t button_get(uint32_t button_idx)
{
    uint32_t pin_set = gpio_read(btn_list[button_idx]) ? 1 : 0;

    return (pin_set == (BUTTONS_ACTIVE_STATE ? 1 : 0));
}

/* Single nop instruction for 1 clock cycle delay */
static inline void nop() { asm("nop"); }

/* Delay in microseconds */
void delay_us(uint32_t usec)
{
    while (usec > 0)
    {
        /*
         * The processor runs at 64 MHz, which is 64 cycles per usec.
         * Overheads associated with a loop, say 4 cycles.
         * Hence, 60 nops below.
         */
        // 1   2      3      4      5      6      7      8
        nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); // 1
        nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); // 2
        nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); // 3
        nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); // 4
        nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); // 5
        nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); // 6
        nop(); nop(); nop(); nop(); nop(); nop(); nop(); nop(); // 7
        nop(); nop(); nop(); nop();

        usec--;
    }
}

/* Delay in milliseconds */
void delay_ms(uint32_t ms)
{
    while (ms > 0)
    {
        delay_us(1000);
        ms--;
    }
}
/* Functions that help me debugging */

void clear_display()
{
    uint32_t r, c;

    for (r = 0; r < LED_NUM_ROWS; r++)
        gpio_clear(led_rows[r]);    // row leds low

    for (c = 0; c < LED_NUM_COLS; c++)
        gpio_set(led_cols[c]);      // column leds high
}

void led_on(int r, int c)
{
    gpio_clear(led_cols[c]);
    gpio_set(led_rows[r]);
}

void led_off(int r, int c)
{
    gpio_set(led_cols[c]);
    gpio_clear(led_rows[r]);
}

/* I found this useful for tracing code execution. Turn on LEDs one by one
 * as you progress through the code till the point of crash.
 */
void led_glow(int r, int c)
{
    gpio_clear(led_cols[c]);
    gpio_set(led_rows[r]);
    delay_ms(500);
}

/* Blink forever, to stop the code flow or handle a fault. */
void led_blink(int r, int c, int count)
{
    gpio_clear(led_cols[c]);
    while ((count == BLINK_FOREVER) || (count > 0))
    {
        gpio_set(led_rows[r]);
        delay_ms(500);
        gpio_clear(led_rows[r]);
        delay_ms(500);
        if (count != BLINK_FOREVER)
            count--;
    }
    gpio_set(led_rows[c]);
}
