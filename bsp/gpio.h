#ifndef GPIO_H
#define GPIO_H
#include <stdint.h>

extern void gpio_in(uint32_t pin, uint32_t pull);
#define   PULL_NONE     0
#define   PULLDOWN      (1 << 2)
#define   PULLUP        (3 << 2)

extern void gpio_out(uint32_t pin, uint32_t drive);
#define DRIVE_STANDARD  0
#define DRIVE_HIGH      (3 << 8)

extern void gpio_write(uint32_t pin, uint32_t val);
extern void gpio_set(uint32_t pin);
extern void gpio_clear(uint32_t pin);
extern uint32_t gpio_read(uint32_t pin);

typedef void (* pfn_t)(void);
void gpio_inten(uint32_t pin, uint32_t event_no, uint32_t edge, pfn_t callback);
#define GPIO_RISINGEDGE     1
#define GPIO_FALLINGEDGE    2
#define GPIO_BOTHEDGES      3

#endif  /* GPIO_H */
