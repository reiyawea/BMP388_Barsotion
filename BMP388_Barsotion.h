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
    struct {
        float t1, t2, t3;
        float p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11;
    } _par;
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
    void ReadRegister(uint8_t reg, uint8_t* buf);
        void WriteRegister(uint8_t reg, uint8_t data);
        inline void ReadCalibrationData();
        //Функция AdvancedShift умножает float f на 2 в степени value
        float AdvancedShift(float f, uint8_t value);

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
            data1 = (float)(uncomp_temp - _par.t1);
            data2 = (float)(data1 * _par.t2);
            // Update the compensated temperature in calib structure since
            // this is needed for pressure calculation
            T_lin = data2 + (data1 * data1) * _par.t3;
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
            data1 = _par.p6 * T_lin;
            data2 = _par.p7 * T_lin2;
            data3 = _par.p8 * T_lin3;
            out1 = _par.p5 + data1 + data2 + data3;
            data1 = _par.p2 * T_lin;
            data2 = _par.p3 * T_lin2;
            data3 = _par.p4 * T_lin3;
            out2 = F_uncomp_press * (_par.p1 + data1 + data2 + data3);
            data1 = F_uncomp_press * F_uncomp_press;
            data2 = _par.p9 + _par.p10 * T_lin;
            data3 = data1 * data2;
            data4 = data3 + (data1 * F_uncomp_press) * _par.p11;
            comp_press = out1 + out2 + data4;
            return comp_press;
        }//*/
};
#endif
