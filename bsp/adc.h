#ifndef ADC_H
#define ADC_H
#include <stdint.h>

#define MIC_SAMPLE_RATE 8000

/* Initialize ADC */
void adc_init(uint32_t pin);

/* Read one sample */
uint16_t adc_in(void);

/* Read a buffer of samples */
void adc_read(uint16_t buf[], uint32_t len);

#endif  /* ADC_H */
