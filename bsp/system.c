/*
 * Minimal system.c file to:
 *  1. Select high frequency clock source using external crystal
 *  2. Enable FPU
 */
#define IOREG32(addr) (*((volatile unsigned long *) (addr)))

/* CoProcessor Access Control Register to enable FPU */
#define CPACR       IOREG32(0xE000ED88UL)

/* Register to select high frequency external crystal */
#define CLK_HFCLKSTART  IOREG32(0x40000000UL)

#define __SYSTEM_CLOCK_64M  (64000000UL)

/* We can use SystemCoreClock variable in other modules to find clock value */
unsigned long SystemCoreClock;

/* Many microcontrollers provide multiple sources of clock and allow
 * us to choose clock speed and accuracy.
 *
 * The system typically starts with an internal crystal, often inaccurate.
 * One of the first actions while booting up is to set the clock speed as
 * desired for our application.
 *
 * SystemCoreClockUpdate function is meant to set the clock speed as required.
 * Here, we are enabling external crystal to get better accuracy, required for
 * accurate peripheral clocks such as for uart baud rate.
 */

static void SystemCoreClockUpdate(void)
{
    CLK_HFCLKSTART = 1;
    SystemCoreClock = __SYSTEM_CLOCK_64M;
}

void SystemInit(void)
{
    /* Enable the FPU, by activating coprocessors CP10 and CP11 */
              /* CP10 */    /* CP11 */
    CPACR |= (3UL << 20) | (3UL << 22);

    SystemCoreClockUpdate();
}
