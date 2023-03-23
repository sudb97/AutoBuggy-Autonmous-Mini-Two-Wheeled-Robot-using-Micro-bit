#include <stdint.h>
#include <string.h>
#include "pwm.h"

/*
 * We will use PWM to provide a square wave to the speaker.
 * We can play various tones by setting the frequency of PWM.
 */

#ifndef IOREG32
#define IOREG32(addr) (*(volatile unsigned long *) (addr))
#endif

/*
 * NRF52 has a PWM that has more functionality than what we need.
 * Hence, we are using a subset of the PWM registers here.
 */

/* Register offsets */
#define PWM_STOP       		IOREG32(0x4001C004)     // stop pwm
#define PWM1_STOP       		IOREG32(0x40021004)     // stop pwm

#define PWM_SEQ0START  		IOREG32(0x4001C008)     // start sequence 0
#define PWM1_SEQ0START  		IOREG32(0x40021008)     // start sequence 0

#define PWM_EVENTS_STOPPED  IOREG32(0x4001C104)     // event to indicate PWM stopped
#define PWM1_EVENTS_STOPPED  IOREG32(0x40021104)     // event to indicate PWM stopped

#define PWM_EVENTS_SEQ0END  IOREG32(0x4001C110)     // event to indicate sequnce 0 ended
#define PWM1_EVENTS_SEQ0END  IOREG32(0x40021110)     // event to indicate sequnce 0 ended

#define PWM_EVENTS_PWMEND   IOREG32(0x4001C118)     // event to indicate pwm period ended
#define PWM1_EVENTS_PWMEND   IOREG32(0x40021118)     // event to indicate pwm period ended

#define PWM_ENABLE          IOREG32(0x4001C500)     // enable / disable pwm0
#define PWM1_ENABLE          IOREG32(0x40021500)     // enable / disable pwm0

#define PWM_COUNTERTOP      IOREG32(0x4001C508)     // to decide PWM frequncy
#define PWM1_COUNTERTOP      IOREG32(0x40021508)     // to decide PWM frequncy
                                                    //   countertop = pwm clock / pwm freq
#define PWM_PRESCALER       IOREG32(0x4001C50C)     // divisor for PWM clock (2 power n)
#define PWM1_PRESCALER       IOREG32(0x4002150C)     // divisor for PWM clock (2 power n)

#define PRESCALER_DIV32   5                       //  divide by 32
#define PWM_DECODER         IOREG32(0x4001C510)     // to set indivudial duty cycles of
                                                    //   all 4 PWM channels
                                                    // we will use common configuration
                                                    // to set all 4 PWM to same duty cycle
#define PWM_SEQ0PTR         IOREG32(0x4001C520)     // address where the sequence is stored
#define PWM1_SEQ0PTR         IOREG32(0x40021520)     // address where the sequence is stored

#define PWM_SEQ0CNT         IOREG32(0x4001C524)     // number of values in the sequence
#define PWM1_SEQ0CNT         IOREG32(0x40021524)     // number of values in the sequence

#define PWM_SEQ0REFRESH     IOREG32(0x4001C528)     // additional pwm cycles between two values of the sequence
#define PWM1_SEQ0REFRESH     IOREG32(0x40021528)     // additional pwm cycles between two values of the sequence
                                                     
#define PWM_SEQ0ENDDELAY    IOREG32(0x4001C52C)     // additional pwm cycles at the end
#define PWM1_SEQ0ENDDELAY    IOREG32(0x4002152C)     // additional pwm cycles at the end

#define PWM_PSEL            IOREG32(0x4001C560)     // pin select
#define PWM1_PSEL            IOREG32(0x40021560)     // pin select

static uint16_t s_sequence[2];

/* PWM clock:
 *  PWM frequency = PWM clock / PWM counter
 *  Hence, lowest frequency = pwm clock / max count
 *
 *  Procssor uses a 15-bit counter with max value of 32767.
 *  For PWM clock of 1 MHz, lowest frequency = 1000000/32767 = ~30 Hz
 *  For PWM clock of 0.5 MHz, lowest frequency = 500000/32767 = ~15 Hz
 * Here, we selct 0.5 MHz clock for audio frequencies starting from 20 Hz.
 */
#define PWM_CLK 500000

/* APIs */

void pwm_init(uint32_t pin)
{
    /* PWM clock is 16 MHz. Hence, Prescaler divisor = 32 for 500 kHz clock */
    PWM_PRESCALER = PRESCALER_DIV32;

    /* Connect the port to the pin */
    PWM_PSEL = pin;

    /* Enable PWM */
    PWM_ENABLE = 1;
}


void pwm1_init(uint32_t pin)
{
    /* PWM clock is 16 MHz. Hence, Prescaler divisor = 32 for 500 kHz clock */
    PWM1_PRESCALER = PRESCALER_DIV32;

    /* Connect the port to the pin */
    PWM1_PSEL = pin;

    /* Enable PWM */
    PWM1_ENABLE = 1;
}



/* Generate a PWM wave of the specified duty cycle for the specified duration
 *  duty cycle in integer percentage, for example, 50.
 */
void pwm_out(int freq, int duty, int duration_ms)
{
    int n_pwm_cycles = (freq * duration_ms) / 1000;
    uint16_t countertop;

    /* Set PWM counter for the required frequency. */
    countertop =  PWM_CLK / freq;
    PWM_COUNTERTOP = countertop;

    /* To generate n PWM cycles, we create a sequence of two identical values
     * with (n-2) cycles gap in-between.
     */
    PWM_SEQ0PTR = (uint32_t) s_sequence;
    PWM_SEQ0CNT = 2;
    PWM_SEQ0REFRESH = n_pwm_cycles - 2;

    /* Set count values for specified duty cycle */
    s_sequence[0] = (countertop * duty) / 100;
    s_sequence[1] = (countertop * duty) / 100;

    /* Start sequence 0 */
    PWM_SEQ0START = 1;

    /* Wait for the sequence to complete */
    // while (PWM_EVENTS_SEQ0END == 0);
    // PWM_EVENTS_SEQ0END = 0;

    // /* Stop PWM */
    // PWM_STOP = 1;
    // while (PWM_EVENTS_STOPPED == 0)
    //     ;
    // PWM_EVENTS_STOPPED = 0;
}


void pwm1_out(int freq, int duty, int duration_ms)
{
    int n_pwm_cycles = (freq * duration_ms) / 1000;
    uint16_t countertop;

    /* Set PWM counter for the required frequency. */
    countertop =  PWM_CLK / freq;
    PWM1_COUNTERTOP = countertop;

    /* To generate n PWM cycles, we create a sequence of two identical values
     * with (n-2) cycles gap in-between.
     */
    PWM1_SEQ0PTR = (uint32_t) s_sequence;
    PWM1_SEQ0CNT = 2;
    PWM1_SEQ0REFRESH = n_pwm_cycles - 2;

    /* Set count values for specified duty cycle */
    s_sequence[0] = (countertop * duty) / 100;
    s_sequence[1] = (countertop * duty) / 100;

    /* Start sequence 0 */
    PWM1_SEQ0START = 1;

    /* Wait for the sequence to complete */
    // while (PWM1_EVENTS_SEQ0END == 0);
    // PWM1_EVENTS_SEQ0END = 0;

    // /* Stop PWM */
    // PWM1_STOP = 1;
    // while (PWM1_EVENTS_STOPPED == 0)
    //     ;
    // PWM1_EVENTS_STOPPED = 0;
}

void pwmservo_stop(){

    PWM_STOP = 1;
    PWM1_STOP = 1;
    while (PWM_EVENTS_STOPPED == 0);
    PWM_EVENTS_STOPPED = 0;
   
    while (PWM1_EVENTS_STOPPED == 0);
    PWM1_EVENTS_STOPPED = 0;

}
