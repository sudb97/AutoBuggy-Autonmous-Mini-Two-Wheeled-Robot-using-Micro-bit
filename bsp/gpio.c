#include <stdint.h>
#include "gpio.h"
#include "board.h"
#include "lib.h"

#ifndef IOREG32
#define IOREG32(addr)   (*((volatile long *) (addr)))
#endif

/* Base addresses of GPIO port 0 and GPIO port 1 */
#define GPIO0       0x50000000UL
#define GPIO1       0x50000300UL

/* Register offsets to be added to GPIO0/1 base address */
#define GPIO_OUT    0x504   // write all 32-bits to a port
#define GPIO_OUTSET 0x508   // set pins as per the mask
                            //   1 sets the pin, 0 no effect
#define GPIO_OUTCLR 0x50c   // clear pins as per the mask
                            //   1 clears the pin, 0 no effect
#define GPIO_IN     0x510   // read a port
#define GPIO_DIR    0x514   // set directions of all 32 pins of a port
                            //   0 input, 1 output

/* Configuration pin registers. There is one 32-bit register for each pin.
 * Provides many options, but we use it only for setting pull-up or pull-down
 * resister.
 */
#define GPIO_PINCFG (0x700)

#define PORT(pin)   (((pin) < 32) ? (GPIO0) : (GPIO1))
#define PIN(pin)    (((pin) < 32) ? (pin) : (pin - 32))

/* Set the specific pin as a digital input pin. */
void gpio_in(uint32_t pin, uint32_t pull)
{
    uint32_t baseaddr;

    baseaddr = PORT(pin);
    pin = PIN(pin);

    IOREG32(baseaddr + GPIO_PINCFG + pin*4) = pull;
    /* all other bits (input direction, pins connected, standard drive,
       no sense) in the register have zero bit-values. */
}

/* Set the specific pin as a digital output pin. */
void gpio_out(uint32_t pin, uint32_t drive)
{
    uint32_t baseaddr;
    uint32_t dir = 1;   // output

    baseaddr = PORT(pin);
    pin = PIN(pin);

    IOREG32(baseaddr + GPIO_DIR) |= (1 << pin);

    IOREG32(baseaddr + GPIO_PINCFG + pin*4) = (dir | drive);
    /* all other bits (no pull, pins connected, no sense) in the register
       have zero bit-values. */
}

/* Write the specified value to the specified pin. */
void gpio_write(uint32_t pin, uint32_t val)
{
    uint32_t baseaddr;

    baseaddr = PORT(pin);
    pin = PIN(pin);

    if (val == 1)
        IOREG32(baseaddr + GPIO_OUTSET) = (1 << pin);
    else
        IOREG32(baseaddr + GPIO_OUTCLR) = (1 << pin);
}

/* Set the specified pin (digital high). */
void gpio_set(uint32_t pin)
{
    uint32_t baseaddr;

    baseaddr = PORT(pin);
    pin = PIN(pin);

    IOREG32(baseaddr + GPIO_OUTSET) = (1 << pin);
}

/* Clear the specified pin (digital low). */
void gpio_clear(uint32_t pin)
{
    uint32_t baseaddr;

    baseaddr = PORT(pin);
    pin = PIN(pin);

    IOREG32(baseaddr + GPIO_OUTCLR) = (1 << pin);
}

/* Read the specified pin: 1 for high, 0 for low. */
uint32_t gpio_read(uint32_t pin)
{
    uint32_t baseaddr;

    baseaddr = PORT(pin);
    pin = PIN(pin);

    return ((IOREG32(baseaddr + GPIO_IN) >> pin) & 1UL);
}

/* Interrupt related configuration */

/* GPIOTE registers */
#define GPIOTE_EVENTSIN(i)  IOREG32(0x40006100 + 4*(i))
#define GPIOTE_INTENSET     IOREG32(0x40006304)
#define GPIOTE_CONFIG(i)    IOREG32(0x40006510 + 4*(i))
#define     GPIOTE_MODEEVENT    (1)

/* NVIC registers */
#define NVIC_ISER           IOREG32(0xE000E100)
#define GPIOTE_ID           6               // peripheral ID

typedef void (*pfn_t)(void);
pfn_t event_callbacks[8];

void gpio_inten(uint32_t pin, uint32_t event_no, uint32_t edge, pfn_t callback)
{
   /* function to call back when the event occurs */
   event_callbacks[event_no] = callback;

    /* GPIOTE has 8 registers, each can be configured for event i (i = 0 to 7) 
     * along with the pin number and event type associated with the event.
     */
   GPIOTE_CONFIG(event_no)
        = ((GPIOTE_MODEEVENT) | (pin << 8) | (edge << 16));

   /* Generate an interrupt when the specified event occurs. */
   GPIOTE_INTENSET |= (1 << event_no);

    /* Enable GPIOTE interrupts in the interrupt controller */
    NVIC_ISER |= (1 << GPIOTE_ID);
}

/* use these for debugging */
volatile int gpio_irq_counter;
volatile int gpio_irq_event_counters[8];

void GPIOTE_IRQHandler(void)
{
    int i;

    for (i = 0; i < 8; i++)
    {
        if (GPIOTE_EVENTSIN(i))
        {
            gpio_irq_event_counters[i]++;
            if (event_callbacks[i])
                event_callbacks[i]();
            GPIOTE_EVENTSIN(i) = 0;
        }
    }

    gpio_irq_counter++;
}
