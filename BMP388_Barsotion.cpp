#include "BMP388_Barsotion.h"

//==============================================================================
BMP388_t::BMP388_t()
{
    this->readRegister = BMP388_I2C_readRegister;
    this->writeRegister = BMP388_I2C_writeRegister;
}


bool BMP388_t::init(uint8_t addr)
{
    _address_ = addr;
    BMP388_I2C_Init(addr, 17, 18);
    uint8_t data;
    this->readRegister(BMP388_CHIP_ID, &data, 1);
    if (data != 0x50) return false;
    // Enable Thermometer & Barometer
    data = 0x33;
    this->writeRegister(BMP388_PWR_CTRL, &data, 1);
    ReadCalibrationData();
    return true;
}


uint8_t BMP388_t::getWhoAmI()
{
    uint8_t dummy;
    this->readRegister(BMP388_CHIP_ID, &dummy, 1);
    return dummy;
}
//==============================================================================
void BMP388_t::setTempOvs(uint8_t value)
{
    value &= 0b00000111; //защита от дурака
    value <<= 3;
    osr_reg &= 0b00000111;
    osr_reg |= value;
    this->writeRegister(BMP388_OSR, &osr_reg, 1);
}


void BMP388_t::setPresOvs(uint8_t value)
{
    value &= 0b00000111; //защита от дурака
    osr_reg &= 0b00111000;
    osr_reg |= value;
    this->writeRegister(BMP388_OSR, &osr_reg, 1);
}


void BMP388_t::setIIRFilterCoef(uint8_t value)
{
    value &= 0b00000111; //защита от дурака
    value <<= 1;
    this->writeRegister(BMP388_CONFIG, &value, 1);
}


void BMP388_t::setODR(uint8_t value)
{
    value &= 0b00011111; //защита от дурака
    this->writeRegister(BMP388_ODR, &value, 1);
}


//==============================================================================
void BMP388_t::ReadCalibrationData()
{
    uint8_t data[21];
    this->readRegister(BMP388_T1_L, data, 21);
    _par.t1 = (float)(((uint16_t)data[1] << 8) | ((uint16_t)data[0]));
    _par.t2 = (float)(((uint16_t)data[3] << 8) | ((uint16_t)data[2]));
    _par.t3 = (float)((int8_t)data[4]);
    _par.p1 = (float)(((int16_t)data[6] << 8) | ((int16_t)data[5]));
    _par.p2 = (float)(((int16_t)data[8] << 8) | ((int16_t)data[7]));
    _par.p3 = (float)((int8_t)data[9]);
    _par.p4 = (float)((int8_t)data[10]);
    _par.p5 = (float)(((uint16_t)data[12] << 8) | ((uint16_t)data[11]));
    _par.p6 = (float)(((uint16_t)data[14] << 8) | ((uint16_t)data[13]));
    _par.p7 = (float)((int8_t)data[15]);
    _par.p8 = (float)((int8_t)data[16]);
    _par.p9 = (float)(((int16_t)data[18] << 8) | ((int16_t)data[17]));
    _par.p10 = (float)((int8_t)data[19]);
    _par.p11 = (float)((int8_t)data[20]);
    _par.t1 = AdvancedShift(_par.t1, 8);
    _par.t2 = AdvancedShift(_par.t2, -30);
    _par.t3 = AdvancedShift(_par.t3, -48);
    _par.p1 = AdvancedShift(_par.p1 - 16384.0, -20);
    _par.p2 = AdvancedShift(_par.p2 - 16384.0, -29);
    _par.p3 = AdvancedShift(_par.p3, -32);
    _par.p4 = AdvancedShift(_par.p4, -37);
    _par.p5 = AdvancedShift(_par.p5, 3);
    _par.p6 = AdvancedShift(_par.p6, -6);
    _par.p7 = AdvancedShift(_par.p7, -8);
    _par.p8 = AdvancedShift(_par.p8, -15);
    _par.p9 = AdvancedShift(_par.p9, -48);
    _par.p10 = AdvancedShift(_par.p10, -48);
    _par.p11 = AdvancedShift(_par.p11, -65);
}


/* Функция AdvancedShift умножает float f на 2 в степени value */
float BMP388_t::AdvancedShift(float f, uint8_t value)
{
    //avoid the zero exception
    if (f == 0.0) return f;
    uint8_t* u = (uint8_t*)&f;
    //save the sign
    uint8_t sign = u[3] & 0x80;
    //extract the exp with the sign
    uint16_t exponent = ((uint16_t)u[3]<<8)|((uint16_t)u[2]);
    exponent += (uint16_t)value << 7;
    //clear the sign bit
    exponent &= 0x7FFF;
    //native sign insertion
    exponent |= (uint16_t)sign << 8;
    //save modifications
    u[2] = exponent & 0xFF;
    u[3] = exponent >> 8;
    return f;
}


//==============================================================================
void BMP388_t::calcTemp(uint8_t *raw)
{
    uint32_t uncomp_temp = (uint32_t)raw[2]<<16 | (uint32_t)raw[1]<<8 | raw[0];
    BMP388_compensate_temperature(uncomp_temp);
}


void BMP388_t::calcPres(uint8_t *raw)
{
    uint32_t uncomp_pres = (uint32_t)raw[2]<<16 | (uint32_t)raw[1]<<8 | raw[0];
    BMP388_compensate_pressure(uncomp_pres);
}


void BMP388_t::calcAlt()
{
    this->altitude = 44330.0 * (1.0 - powf(this->pressure / _SEA_LEVEL, 0.1903));
}


void BMP388_t::calcAlt(float p0)
{
    this->altitude = 44330.0 * (1.0 - powf(this->pressure / p0, 0.1903));
}


//==============================================================================
void BMP388_t::BMP388_compensate_temperature(uint32_t uncomp_temp)
{
    float data1;
    float data2;
    data1 = (float)(uncomp_temp - _par.t1);
    data2 = (float)(data1 * _par.t2);
    // Update the compensated temperature in calib structure since
    // this is needed for pressure calculation
    this->temperature = data2 + (data1 * data1) * _par.t3;
}


void BMP388_t::BMP388_compensate_pressure(uint32_t uncomp_press)
{
    // Temporary variables used for compensation
    float data1;
    float data2;
    float data3;
    float data4;
    float out1;
    float out2;
    float T_lin2 = this->temperature * this->temperature;
    float T_lin3 = T_lin2 * this->temperature;
    float F_uncomp_press = (float)uncomp_press;
    // Calibration data
    data1 = _par.p6 * this->temperature;
    data2 = _par.p7 * T_lin2;
    data3 = _par.p8 * T_lin3;
    out1 = _par.p5 + data1 + data2 + data3;
    data1 = _par.p2 * this->temperature;
    data2 = _par.p3 * T_lin2;
    data3 = _par.p4 * T_lin3;
    out2 = F_uncomp_press * (_par.p1 + data1 + data2 + data3);
    data1 = F_uncomp_press * F_uncomp_press;
    data2 = _par.p9 + _par.p10 * this->temperature;
    data3 = data1 * data2;
    data4 = data3 + (data1 * F_uncomp_press) * _par.p11;
    this->pressure = out1 + out2 + data4;
}
