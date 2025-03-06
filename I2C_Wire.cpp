#include "BMP388_Interface.h"
#include "Wire.h"


uint8_t __I2C_Wire_address;

uint8_t BMP388_I2C_Init(uint8_t addr, int sda, int scl)
{
    Wire.begin(sda, scl);
    __I2C_Wire_address = addr;
    return 0;
}


uint8_t BMP388_I2C_readRegister(uint8_t reg, uint8_t *buf, uint8_t size)
{
    Wire.beginTransmission(__I2C_Wire_address);
    Wire.write(reg);
    Wire.endTransmission(false);
    Wire.requestFrom(__I2C_Wire_address, size, (uint8_t)true);
    for (uint8_t i=0; i<size; i++)
    {
        buf[i] = Wire.read();
    }
    Wire.endTransmission(true);
    return 0;
}


uint8_t BMP388_I2C_writeRegister(uint8_t reg, uint8_t *buf, uint8_t size)
{
    Wire.beginTransmission(__I2C_Wire_address);
    Wire.write(reg);
    for (uint8_t i=0; i<size; i++)
    {
        Wire.write(buf[i]);
    }
    Wire.endTransmission(true);
    return 0;
}
