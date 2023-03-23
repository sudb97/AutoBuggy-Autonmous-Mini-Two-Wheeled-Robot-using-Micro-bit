#include <stdint.h>
#include <string.h>
#include "adc.h"

/*
 * We will use an ADC to read inputs from the microphone.
 */

#ifndef IOREG32
#define IOREG32(addr) (*(volatile unsigned long *) (addr))
#endif

/* We are using only a subset of registers relevant to us to keep things
 * simple. Hence all registers are not defined below.
 */

/* Register offsets */
#define ADC_START           IOREG32(0x40007000)
#define ADC_SAMPLE          IOREG32(0x40007004)
#define ADC_STOP            IOREG32(0x40007008)
#define ADC_EVENTS_END      IOREG32(0x40007104) // sampling has filled up the result buffer
#define ADC_EVENTS_DONE     IOREG32(0x40007108) // conversion task completed
#define ADC_EVENTS_RESDONE  IOREG32(0x4000710C) // result ready for transfer to RAM
#define ADC_STATUS          IOREG32(0x40007400) // indicates if busy or conversion done
#define ADC_ENABLE          IOREG32(0x40007500) // enable/disable ADC
#define ADC_PSELP(n)        IOREG32(0x40007510 + n * 0x10)
                                                // channel n pin: ain0 to ain7 (not port pins)
#define ADC_CONFIG(n)       IOREG32(0x40007518 + n * 0x10)
                                                // channel n configuration
#define ADC_RESOLUTION      IOREG32(0x400075F0) // bits per sample
#define     ADC_RES8BIT     0
#define     ADC_RES10BIT    1
#define     ADC_RES12BIT    2
#define     ADC_RES14BIT    3

#define ADC_SAMPLERATE      IOREG32(0x400075F8) // sampling rate
#define     ADC_SAMPLEMODE_LOCAL    (1 << 12)   // controlled by local timer
#define ADC_RESULT_PTR      IOREG32(0x4000762C) // pointer to write the result
#define ADC_RESULT_CNT      IOREG32(0x40007630) // number of samples to write
#define ADC_RESULT_AMT      IOREG32(0x40007634) // number of samples to write

#define CLK_16MHZ   16000000UL

void adc_init(uint32_t ain)
{
    uint16_t cc;    // compare counter for sampling

    /* Enable ADC */
    ADC_ENABLE = 1;

    /*
     * Connect channel 0 to the specified analog input pin ain.
     * (value 0 for disconnected and ain + 1 to connect to ain pin)
     */
    ADC_PSELP(0) = ain + 1;

    /* Configure parameters */
    ADC_CONFIG(0) = ((7 << 8) | (2 << 16));   // gain = 4, tacq = 10us
    cc = CLK_16MHZ / MIC_SAMPLE_RATE;
    ADC_SAMPLERATE = ADC_SAMPLEMODE_LOCAL | cc;

    /* Set ADC parameters */
    ADC_RESOLUTION = ADC_RES14BIT;
}

/* Read one sample from ADC */
uint16_t adc_in(void)
{
    volatile uint16_t sample[1];    // changed by DMA

    adc_read((uint16_t *) sample, 1);

    return sample[0];
}

/* Read a buffer of samples from ADC */
void adc_read(uint16_t buf[], uint32_t len)
{
#if 0   // for testing: corrupt the buffer to ensure that
        // the values were really written by the ADC.
    int i;

    for (i = 0; i < len; i++)
        buf[len] = 0xabcd;
#endif

    /* Set up result buffer for DMA */
    ADC_RESULT_PTR = (uint32_t) buf;
    ADC_RESULT_CNT = len;

    /* Start the ADC and start sampling the first input. */
    ADC_START = 1;
    ADC_SAMPLE = 1;

    /* Wait until all the samples are written into memory. */
    while (ADC_EVENTS_END == 0)
        ;
    ADC_EVENTS_END = 0;     // clear the event

#if 0   // testing
    printf("%d number of samples\n", ADC_RESULT_AMT);
    for (i = 0; i < len; i++)
        printf("%x\n", buf[i]);
#endif

    return;
}
