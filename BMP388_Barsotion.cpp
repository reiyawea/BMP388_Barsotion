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
        inline void ReadCalibrationData() {
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
            Par_T1 = (float)(((uint16_t)data[1] << 8) | ((uint16_t)data[0]));
            Par_T2 = (float)(((uint16_t)data[3] << 8) | ((uint16_t)data[2]));
            Par_T3 = (float)((int8_t)data[4]);
            Par_P1 = (float)(((int16_t)data[6] << 8) | ((int16_t)data[5]));
            Par_P2 = (float)(((int16_t)data[8] << 8) | ((int16_t)data[7]));
            Par_P3 = (float)((int8_t)data[9]);
            Par_P4 = (float)((int8_t)data[10]);
            Par_P5 = (float)(((uint16_t)data[12] << 8) | ((uint16_t)data[11]));
            Par_P6 = (float)(((uint16_t)data[14] << 8) | ((uint16_t)data[13]));
            Par_P7 = (float)((int8_t)data[15]);
            Par_P8 = (float)((int8_t)data[16]);
            Par_P9 = (float)(((int16_t)data[18] << 8) | ((int16_t)data[17]));
            Par_P10 = (float)((int8_t)data[19]);
            Par_P11 = (float)((int8_t)data[20]);
            Par_T1 = AdvancedShift(Par_T1, 8);
            Par_T2 = AdvancedShift(Par_T2, -30);
            Par_T3 = AdvancedShift(Par_T3, -48);
            Par_P1 = AdvancedShift(Par_P1 - 16384.0, -20);
            Par_P2 = AdvancedShift(Par_P2 - 16384.0, -29);
            Par_P3 = AdvancedShift(Par_P3, -32);
            Par_P4 = AdvancedShift(Par_P4, -37);
            Par_P5 = AdvancedShift(Par_P5, 3);
            Par_P6 = AdvancedShift(Par_P6, -6);
            Par_P7 = AdvancedShift(Par_P7, -8);
            Par_P8 = AdvancedShift(Par_P8, -15);
            Par_P9 = AdvancedShift(Par_P9, -48);
            Par_P10 = AdvancedShift(Par_P10, -48);
            Par_P11 = AdvancedShift(Par_P11, -65);
        }//*/
        //Функция AdvancedShift умножает float f на 2 в степени value
float BMP388_t::AdvancedShift(float f, uint8_t value) {
    //избегаем исключения с нулем
    if (f == 0.0) return f;
    uint8_t* u = (uint8_t*)&f;
    //сохранить знак
    uint8_t sign = u[3] & 0x80;
    //выделяем порядок со знаком
    uint16_t exponent = ((uint16_t)u[3]<<8)|((uint16_t)u[2]);
    exponent += (uint16_t)value << 7;
    //очистка разряда знака
    exponent &= 0x7FFF;
    //вставка начального знака
    exponent |= (uint16_t)sign << 8;
    //сохранить изменения
    u[2] = exponent & 0xFF;
    u[3] = exponent >> 8;
    return f;
}//*/
