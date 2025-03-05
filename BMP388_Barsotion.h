/*
    (E) Barsotion KY
    Библиотека BMP388
    v1.3
*/
#pragma once

#include "BMP388_RegMap.h"
#include "BMP388_Interface.h"

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
    struct
    {
        float t1, t2, t3;
        float p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11;
    } _par;
    uint8_t osr_reg;
    bool temp_flag;
    bool press_flag;
    uint32_t raw[2];
public:
    float temperature, pressure, altitude;
    uint8_t (*readRegister)(uint8_t reg, uint8_t *buf, uint8_t size);
    uint8_t (*writeRegister)(uint8_t reg, uint8_t *buf, uint8_t size);

public:
    BMP388_t();
    
    bool init(uint8_t addr);
    void setTempOvs(uint8_t value);
    void setPresOvs(uint8_t value);
    void setIIRFilterCoef(uint8_t value);
    void setODR(uint8_t value);
        
    void ReadCalibrationData();
    //Функция AdvancedShift умножает float f на 2 в степени value
    float AdvancedShift(float f, uint8_t value);

    void calcTemp(uint8_t *raw);
    void calcPres(uint8_t *raw);
    void calcAlt();
    void calcAlt(float p0);
        
private:
    void BMP388_compensate_temperature(uint32_t uncomp_temp);
    void BMP388_compensate_pressure(uint32_t uncomp_press);
};
#endif
