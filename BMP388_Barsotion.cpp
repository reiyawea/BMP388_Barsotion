#include "BMP388_Barsotion.h"


bool BMP388_t::init(uint8_t addr)
{
    _address_ = addr;
    uint8_t data;
    ReadRegister(BMP388_CHIP_ID, &data);
    if (data != 0x50) return false;
    // Enable Thermometer & Barometer
    WriteRegister(BMP388_PWR_CTRL, 0x33);
    ReadCalibrationData();
    return true;
}
    

void BMP388_t::setTempOvs(uint8_t value)
{
    value &= 0b00000111; //защита от дурака
    value <<= 3;
    osr_reg &= 0b00000111;
    osr_reg |= value;
    WriteRegister(BMP388_OSR, osr_reg);
}


void BMP388_t::setPresOvs(uint8_t value)
{
    value &= 0b00000111; //защита от дурака
    osr_reg &= 0b00111000;
    osr_reg |= value;
    WriteRegister(BMP388_OSR, osr_reg);
}


void BMP388_t::setIIRFilterCoef(uint8_t value)
{
    value &= 0b00000111; //защита от дурака
    value <<= 1;
    WriteRegister(BMP388_CONFIG, value);
}


void BMP388_t::setODR(uint8_t value)
{
    value &= 0b00011111; //защита от дурака
    WriteRegister(BMP388_ODR, value);
}


//==============================================================================
void ReadRegister(uint8_t reg, uint8_t* buf) {
            Wire.beginTransmission(_address_);
            Wire.write(reg);
            Wire.endTransmission(false);
            Wire.requestFrom(_address_, (uint8_t)1, (uint8_t)true);
            *buf = Wire.read();
            Wire.endTransmission(true);
        }
        void WriteRegister(uint8_t reg, uint8_t data) {
            Wire.beginTransmission(_address_);
            Wire.write(reg);
            Wire.write(data);
            Wire.endTransmission(true);
        }
void BMP388_t::ReadCalibrationData()
{
    uint8_t data[21];
    Wire.beginTransmission(_address_);
    Wire.write(BMP388_T1_L);
    Wire.endTransmission(false);
    Wire.requestFrom(_address_, (uint8_t)21, (uint8_t)true);
    for (uint8_t i=0; i<21; i++) data[i] = Wire.read();
    Wire.endTransmission(true);
    //Преобразование получаемых с датчика констант в параметры коррекции
    // TODO: избавиться от массива data, сделать последовательный
    // вызов функции Wire.read() Upd: если так сделать, программа
    // занимает на 72 байта больше места в памяти
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


