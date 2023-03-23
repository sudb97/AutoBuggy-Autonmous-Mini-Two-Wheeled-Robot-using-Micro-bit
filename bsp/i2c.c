#include <stdint.h>

#define IOREG32(addr)   (*(volatile uint32_t *)(addr))

#define I2C0_STARTRX    IOREG32(0x40003000)
#define I2C0_STARTTX    IOREG32(0x40003008)
#define I2C0_STOP       IOREG32(0x40003014)
#define I2C0_STOPPED    IOREG32(0x40003104)
#define I2C0_RXDREADY   IOREG32(0x40003108)
#define I2C0_TXDSENT    IOREG32(0x4000311c)
#define I2C0_SHORTS     IOREG32(0x40003200)
#define   I2C_BB_STOP   (1 << 1)
#define I2C0_ENABLE     IOREG32(0x40003500)
#define   I2C_ENABLE_Disabled   0
#define   I2C_ENABLE_Enabled    5
#define I2C0_PSELSCL    IOREG32(0x40003508)
#define I2C0_PSELSDA    IOREG32(0x4000350C)
#define I2C0_RXD        IOREG32(0x40003518)
#define I2C0_TXD        IOREG32(0x4000351C)
#define I2C0_FREQUENCY  IOREG32(0x40003524)
#define   I2C_FREQUENCY_100kHz  0x01980000
#define I2C0_ADDRESS    IOREG32(0x40003588)

/* i2c_init -- start I2C driver process */
void i2c_init(uint32_t scl_pin, uint32_t sda_pin)
{
    // Configure I2C hardware
    I2C0_PSELSCL = scl_pin;
    I2C0_PSELSDA = sda_pin;
    I2C0_FREQUENCY = I2C_FREQUENCY_100kHz;
    I2C0_ENABLE = I2C_ENABLE_Enabled;
}

char i2c_read_reg(int addr, int cmd)
{
    char ch;

    /* Send command */
    I2C0_ADDRESS = addr >> 1;
    I2C0_STARTTX = 1;
    I2C0_TXD = cmd;

    /* Wait till tx completes */
    while (I2C0_TXDSENT == 0)
        ;
    I2C0_TXDSENT = 0;

    I2C0_SHORTS = I2C_BB_STOP;  // STOP to send a NACK at the end
    I2C0_STARTRX = 1;

    while (I2C0_RXDREADY == 0)
        ;
    I2C0_RXDREADY = 0;

    ch = I2C0_RXD;

    /* Stop */
    I2C0_STOP = 1;
    while (I2C0_STOPPED == 0)
        ;
    I2C0_STOPPED = 0;

    I2C0_SHORTS = 0;

    return ch;
}

/* i2c_write_reg -- send command and write data */
void i2c_write_reg(int addr, int cmd, char val)
{
    I2C0_ADDRESS = addr >> 1;
    I2C0_STARTTX = 1;

    /* Send command */
    I2C0_TXD = cmd;
    while (I2C0_TXDSENT == 0)   // wait till tx completes
        ;
    I2C0_TXDSENT = 0;

    /* Send value */
    I2C0_TXD = val;
    while (I2C0_TXDSENT == 0)   // wait till tx completes
        ;
    I2C0_TXDSENT = 0;

    /* Stop */
    I2C0_STOP = 1;
    while (I2C0_STOPPED == 0)
        ;
    I2C0_STOPPED = 0;

    return;
}
