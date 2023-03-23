#ifndef I2C_H
#define I2C_H
#include <stdint.h>

extern void i2c_init(uint32_t scl_pin, uint32_t sda_pin);
extern char i2c_read_reg(int addr, int cmd);
extern void i2c_write_reg(int addr, int cmd, char val);

#endif  /* I2C_H */
