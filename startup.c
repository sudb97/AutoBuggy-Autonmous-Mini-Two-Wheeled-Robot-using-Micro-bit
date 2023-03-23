#include <stdint.h>

//*****************************************************************************
// Functions defined in this file
//*****************************************************************************
void ResetISR(void);
static void crt_init(void);
static void HardFaultHandler(void);
static void BusFaultHandler(void);
static void UsageFaultHandler(void);

void GPIOTE_IRQHandler(void);
static void IntDefaultHandler(void);

void __attribute__((weak)) SysTick_Handler(void) { IntDefaultHandler(); }
void __attribute__((weak)) SVC_Handler(void) { IntDefaultHandler(); }
void __attribute__((weak)) PendSV_Handler(void) { IntDefaultHandler(); }





//*****************************************************************************
// The following are constructs created by the linker, indicating where the
// the "data" and "bss" segments reside in memory.  The initializers for the
// for the "data" segment resides immediately following the "text" segment.
//*****************************************************************************
extern uint32_t __data_load__;  // load address of initialized data in ROM
extern uint32_t __data_start__; // start address of initialized data in RAM
extern uint32_t __data_end__;   // end address of initialized data in RAM
extern uint32_t __bss_start__;  // start address of uninitialized data in RAM
extern uint32_t __bss_end__;    // end address of uninitialized data in RAM
extern uint32_t __stack_end__;  // address of stack end in RAM

extern int SystemInit(void);    // system initialization function
extern int main(void);          // application main function

typedef void (*pfn_t)(void);

//*****************************************************************************
// Vector table
//*****************************************************************************
__attribute__ ((section(".vectors")))
pfn_t vector_table[] =
{
    /* Core exceptions */
    (pfn_t) (long) (&__stack_end__),        // The initial stack pointer
    ResetISR,                               // The reset handler
    IntDefaultHandler,                      // The NMI handler
    HardFaultHandler,                       // The hard fault handler
    IntDefaultHandler,                      // The MPU fault handler
    BusFaultHandler,                        // The bus fault handler
    UsageFaultHandler,                      // The usage fault handler
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    SVC_Handler,                            // SVC_Handler
    IntDefaultHandler,                      // Debug monitor handler
    0,                                      // Reserved
    PendSV_Handler,                        // The PendSV_ handler
    SysTick_Handler,                         // The SysTick handler
    /* Peripheral interrupts */
    IntDefaultHandler,                      // POWER_CLOCK_IRQHandler
    IntDefaultHandler,                      // RADIO_IRQHandler
    IntDefaultHandler,                      // UARTE0_UART0_IRQHandler
    IntDefaultHandler,                      // SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQHandler
    IntDefaultHandler,                      // SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQHandler
    IntDefaultHandler,                      // NFCT_IRQHandler
    GPIOTE_IRQHandler,                      // GPIOTE_IRQHandler
    IntDefaultHandler,                      // SAADC_IRQHandler
    IntDefaultHandler,                      // TIMER0_IRQHandler
    IntDefaultHandler,                      // TIMER1_IRQHandler
    IntDefaultHandler,                      // TIMER2_IRQHandler
    IntDefaultHandler,                      // RTC0_IRQHandler
    IntDefaultHandler,                      // TEMP_IRQHandler
    IntDefaultHandler,                      // RNG_IRQHandler
    IntDefaultHandler,                      // ECB_IRQHandler
    IntDefaultHandler,                      // CCM_AAR_IRQHandler
    IntDefaultHandler,                      // WDT_IRQHandler
    IntDefaultHandler,                      // RTC1_IRQHandler
    IntDefaultHandler,                      // QDEC_IRQHandler
    IntDefaultHandler,                      // COMP_LPCOMP_IRQHandler
    IntDefaultHandler,                      // SWI0_EGU0_IRQHandler
    IntDefaultHandler,                      // SWI1_EGU1_IRQHandler
    IntDefaultHandler,                      // SWI2_EGU2_IRQHandler
    IntDefaultHandler,                      // SWI3_EGU3_IRQHandler
    IntDefaultHandler,                      // SWI4_EGU4_IRQHandler
    IntDefaultHandler,                      // SWI5_EGU5_IRQHandler
    IntDefaultHandler,                      // TIMER3_IRQHandler
    IntDefaultHandler,                      // TIMER4_IRQHandler
    IntDefaultHandler,                      // PWM0_IRQHandler
    IntDefaultHandler,                      // PDM_IRQHandler
    0,                                      // Reserved
    0,                                      // Reserved
    IntDefaultHandler,                      // MWU_IRQHandler
    IntDefaultHandler,                      // PWM1_IRQHandler
    IntDefaultHandler,                      // PWM2_IRQHandler
    IntDefaultHandler,                      // SPIM2_SPIS2_SPI2_IRQHandler
    IntDefaultHandler,                      // RTC2_IRQHandler
    IntDefaultHandler,                      // I2S_IRQHandler
    IntDefaultHandler,                      // FPU_IRQHandler
    IntDefaultHandler,                      // USBD_IRQHandler
    IntDefaultHandler,                      // UARTE1_IRQHandler
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    IntDefaultHandler,                      // PWM3_IRQHandler
    0,                                      // Reserved
    IntDefaultHandler,                      // SPIM3_IRQHandler
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0                                       // Reserved
};

void ResetISR(void)
{
    crt_init();
    SystemInit();

    main();

    while (1)
        ;   // loop forever
}

static void crt_init(void)
{
    uint32_t *pui32Src, *pui32Dest;

    // Copy the data segment initializers from flash to SRAM.
    pui32Src = &__data_load__;
    pui32Dest = &__data_start__;
    while(pui32Dest < &__data_end__)
        *pui32Dest++ = *pui32Src++;

    // Zero fill the bss segment.
    for(pui32Dest = &__bss_start__; pui32Dest < &__bss_end__; pui32Dest++)
        *pui32Dest = 0;

    return;
}

/* For debugging */
extern void led_blink(int r, int c, int count);
#define BLINK_FOREVER -1

static void HardFaultHandler(void) { led_blink(4, 0, BLINK_FOREVER); }
static void BusFaultHandler(void) { led_blink(4, 1, BLINK_FOREVER); }
static void UsageFaultHandler(void) { led_blink(4, 2, BLINK_FOREVER); }

void __attribute__((weak)) GPIOTE_IRQHandler(void) { IntDefaultHandler(); }
static void IntDefaultHandler(void) { led_blink(4, 4, BLINK_FOREVER); }
