#include "timer.h"

#define IOREG32(addr)   (*(volatile unsigned long *)(addr))

#define SYSTICK_CSR     IOREG32(0xE000E010)
#define SYSTICK_RVR     IOREG32(0xE000E014)
#define SYSTICK_CVR     IOREG32(0xE000E018)

#define NUM_TIMERS 3

typedef void (*pfn)(void);

struct{
    uint32_t count;
    uint32_t reload;
    pfn callback;
} timers[3];

int ticks;
void SysTickHandler(void)
{
    int n;

    for (n = 0; n < NUM_TIMERS; n++)
    {
        if (timers[n].reload != 0)  // timer is active
        {
            timers[n].count--;
            if (timers[n].count == 0)
            {
                timers[n].count = timers[n].reload;
                timers[n].callback();
            }
        }
    }
}

/* 1 millisecond counter value for 64MHz processor clock */
#define TICKS_1MS (64000000L/1000)

void timer_init()
{
    int n;

    /* set all timers to inactive */
    for (n = 0; n < NUM_TIMERS; n++)
        timers[n].reload = 0;   // inactive

    /* enable systick */
    SYSTICK_RVR = TICKS_1MS;
    SYSTICK_CSR = 7;            // clock source internal, interrupts enabled
}

void timer_start(uint32_t n, uint32_t reload_value, pfn_t callback)
{
    timers[n].reload = reload_value;
    timers[n].count = reload_value;
    timers[n].callback = callback;
}

void timer_stop(uint32_t n)
{
    timers[n].reload = 0;
}
