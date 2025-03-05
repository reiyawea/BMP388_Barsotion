/*
    (E) Barsotion KY
    Библиотека BMP388
    v1.3
*/
#pragma once

#include "BMP388_RegMap.h"

#define _SEA_LEVEL                   101325.0

#define BMP3_OVERSAMPLING_1X        0b000
#define BMP3_OVERSAMPLING_2X        0b001
#define BMP3_OVERSAMPLING_4X        0b010
#define BMP3_OVERSAMPLING_8X        0b011
#define BMP3_OVERSAMPLING_16X       0b100
#define BMP3_OVERSAMPLING_32X       0b101

#define BMP3_IIR_FILTER_COEFF_0     0b000
#define BMP3_IIR_FILTER_COEFF_1     0b001
#define BMP3_IIR_FILTER_COEFF_3     0b010
#define BMP3_IIR_FILTER_COEFF_7     0b011
#define BMP3_IIR_FILTER_COEFF_15    0b100
#define BMP3_IIR_FILTER_COEFF_31    0b101
#define BMP3_IIR_FILTER_COEFF_63    0b110
#define BMP3_IIR_FILTER_COEFF_127   0b111

#define BMP3_ODR_200_HZ             0x00
#define BMP3_ODR_100_HZ             0x01
#define BMP3_ODR_50_HZ              0x02
#define BMP3_ODR_25_HZ              0x03
#define BMP3_ODR_12p5_HZ            0x04
#define BMP3_ODR_6p25_HZ            0x05
#define BMP3_ODR_3p1_HZ             0x06
#define BMP3_ODR_1p5_HZ             0x07
#define BMP3_ODR_0p78_HZ            0x08
#define BMP3_ODR_0p39_HZ            0x09
#define BMP3_ODR_0p2_HZ             0x0A
#define BMP3_ODR_0p1_HZ             0x0B
#define BMP3_ODR_0p05_HZ            0x0C
#define BMP3_ODR_0p02_HZ            0x0D
#define BMP3_ODR_0p01_HZ            0x0E
#define BMP3_ODR_0p006_HZ           0x0F
#define BMP3_ODR_0p003_HZ           0x10
#define BMP3_ODR_0p0015_HZ          0x11

#define TEMP_FLAG                   0x01
#define PRESS_FLAG                  0x02


class BMP388_t
{
private:
    uint8_t _address_;
    float Par_T1, Par_T2, Par_T3, Par_P1, Par_P2, Par_P3, Par_P4;
    float Par_P5, Par_P6, Par_P7, Par_P8, Par_P9, Par_P10, Par_P11;
    float T_lin, comp_press;
    uint8_t osr_reg;
    bool temp_flag;
    bool press_flag;
    uint32_t raw[2];
    
public:
    uint8_t (*readRegister)(uint8_t reg, uint8_t *buf, uint8_t size);
    uint8_t (*writeRegister)(uint8_t reg, uint8_t *buf, uint8_t size);

public:
    BMP388_t();
    
    bool init(uint8_t addr);
    void setTempOvs(uint8_t value);
    void setPresOvs(uint8_t value);
    void setIIRFilterCoef(uint8_t value);
    void setODR(uint8_t value);
        
    public:
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
        float AdvancedShift(float f, uint8_t value) {
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

    public:
        void performReading() {
            //Чтение сырых данных
            //uint32_t raw[2];
            Wire.beginTransmission(_address_);
            Wire.write(BMP388_PRESS_XLSB);
            Wire.endTransmission(false);
            Wire.requestFrom((uint8_t)_address_, (uint8_t)6, (uint8_t)true);
            raw[0] = (uint32_t)Wire.read() | ((uint32_t)Wire.read()<<8) |
                ((uint32_t)Wire.read()<<16);
            raw[1] = (uint32_t)Wire.read() | ((uint32_t)Wire.read()<<8) |
                ((uint32_t)Wire.read()<<16);
            Wire.endTransmission(true);
            press_flag = false;
            temp_flag = false;
            //Обработка данных
            //<>
        }
        volatile float readTemperature() {
            temp_flag = true;
            return BMP388_compensate_temperature(raw[1]);
        }
        volatile float readPressure() {
            press_flag - true;
            if (temp_flag == false) {
                T_lin = readTemperature();
            }
            return BMP388_compensate_pressure(raw[0]);
        }
        float readAltitude() {
            if (press_flag == false) {
                comp_press = readPressure();
            }
            return 44330.0 * (1.0 - pow(comp_press / _SEA_LEVEL, 0.1903));
        }
        float readAltitude(float p0) {
            if (press_flag == false) {
                comp_press = readPressure();
            }
            return 44330.0 * (1.0 - pow(comp_press / p0, 0.1903));
        }
        float readBasePressure() {
            /*setTemperatureOversampling(0x011); //плодитель багов
            setPressureOversampling(0x011);
            setOutputDataRate(BMP3_ODR_12p5_HZ);
            setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_127);*/
            /*float p0 = 0;
            _delay_ms(4000);
            for (uint8_t i=0; i<20; i++) {
                performReading();
                p0 += readPressure();
                _delay_ms(100);
            }
            return p0/20;//*/
            performReading();
            readPressure();
            _delay_ms(1000);
            //delay(1000);
            performReading();
            return readPressure();//*/
        }
        
    private:
        float BMP388_compensate_temperature(uint32_t uncomp_temp) {
            float data1;
            float data2;
            data1 = (float)(uncomp_temp - Par_T1);
            data2 = (float)(data1 * Par_T2);
            // Update the compensated temperature in calib structure since
            // this is needed for pressure calculation
            T_lin = data2 + (data1 * data1) * Par_T3;
            // Returns compensated temperature
            return T_lin;
        }
        float BMP388_compensate_pressure(uint32_t uncomp_press) {
            // Temporary variables used for compensation
            float data1;
            float data2;
            float data3;
            float data4;
            float out1;
            float out2;
            float T_lin2 = T_lin * T_lin;
            float T_lin3 = T_lin2 * T_lin;
            float F_uncomp_press = (float)uncomp_press;
            // Calibration data
            data1 = Par_P6 * T_lin;
            data2 = Par_P7 * T_lin2;
            data3 = Par_P8 * T_lin3;
            out1 = Par_P5 + data1 + data2 + data3;
            data1 = Par_P2 * T_lin;
            data2 = Par_P3 * T_lin2;
            data3 = Par_P4 * T_lin3;
            out2 = F_uncomp_press * (Par_P1 + data1 + data2 + data3);
            data1 = F_uncomp_press * F_uncomp_press;
            data2 = Par_P9 + Par_P10 * T_lin;
            data3 = data1 * data2;
            data4 = data3 + (data1 * F_uncomp_press) * Par_P11;
            comp_press = out1 + out2 + data4;
            return comp_press;
        }//*/
};
#endif
