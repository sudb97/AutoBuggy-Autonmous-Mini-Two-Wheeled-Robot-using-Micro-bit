#ifndef PWM_H
#define PWM_H
#include <stdint.h>

void pwm_init(uint32_t pin);
void pwm1_init(uint32_t pin);
void pwmservo_stop();
void pwm_out(int freq, int duty, int duration_ms);
void pwm1_out(int freq, int duty, int duration_ms);
#endif  /* PWM_H */
