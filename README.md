# Arduino library for BMP388 barometer

![](./files/pic1.jpg)
***
## Methods
### `BMP388_t`
Constructor.
### `uint8_t init(uint8_t addr, int sda, int scl)`
Initializes the sensor. Returns 0 if OK, other value if error.
### `uint8_t readRegister(uint8_t reg, uint8_t *buf, uint8_t size)`
Reading BMP388's registers. Returns 0 if OK, other value if error.
### `uint8_t writeRegister(uint8_t reg, uint8_t *buf, uint8_t size)`
Writing BMP388's registers. Returns 0 if OK, other value if error.
### `uint8_t getWhoAmI()`
The sensor's ID should be 0x50.
### `uint8_t setTempOvs(uint8_t ovs)`
Sets temperature oversampling. Allowed values:
- `BMP388_t::OVS_1X`
- `BMP388_t::OVS_2X`
- `BMP388_t::OVS_4X`
- `BMP388_t::OVS_8X`
- `BMP388_t::OVS_16X`
- `BMP388_t::OVS_32X`

Returns 0 if OK, other value if error.
### `uint8_t setPresOvs(uint8_t ovs)`
Sets pressure oversampling. Allowed values are the same as for `setTempOvs` function. Returns 0 if OK, other value if error.
### `uint8_t setIIRFilterCoef(uint8_t coef)`
Sets IIR filter coefficient. Note that bigger values give more accurate result, however add the data delay. Allowed values:
- `BMp388_t::IIR_FILTER_COEF_0`
- `BMp388_t::IIR_FILTER_COEF_1`
- `BMp388_t::IIR_FILTER_COEF_3`
- `BMp388_t::IIR_FILTER_COEF_7`
- `BMp388_t::IIR_FILTER_COEF_15`
- `BMp388_t::IIR_FILTER_COEF_31`
- `BMp388_t::IIR_FILTER_COEF_63`
- `BMp388_t::IIR_FILTER_COEF_127`

Returns 0 if OK, other value if error.
### `uint8_t setODR(uint8_t odr)`
Sets the sensor's ODR value. Allowed values:
- `BMP388_t::ODR_200_HZ`
- `BMP388_t::ODR_100_HZ`
- `BMP388_t::ODR_50_HZ`
- `BMP388_t::ODR_25_HZ`
- `BMP388_t::ODR_12p5_HZ`
- `BMP388_t::ODR_6p25_HZ`
- `BMP388_t::ODR_3p1_HZ`
- `BMP388_t::ODR_1p5_HZ`
- `BMP388_t::ODR_0p78_HZ`
- `BMP388_t::ODR_0p39_HZ`
- `BMP388_t::ODR_0p2_HZ`
- `BMP388_t::ODR_0p1_HZ`
- `BMP388_t::ODR_0p05_HZ`
- `BMP388_t::ODR_0p02_HZ`
- `BMP388_t::ODR_0p01_HZ`
- `BMP388_t::ODR_0p006_HZ`
- `BMP388_t::ODR_0p003_HZ`
- `BMP388_t::ODR_0p0015_HZ`

Returns 0 if OK, other value if error.
### `void calcTemp(uint8_t *raw)`
Calculates temperature. `raw[0]` should contain the `BMP388_TEMP_XLSB` register value, `raw[1]` - `BMP388_TEMP_LSB`, `raw[2]` - `BMP388_TEMP_MSB`.
### `void calcPres(uint8_t *raw)`
Calculates pressure. `raw[0]` should contain the `BMP388_PRESS_XLSB` register value, `raw[1]` - `BMP388_PRESS_LSB`, `raw[2]` - `BMP388_PRESS_MSB`.
### `void calcAlt()`
Calculates altitude from sea level value (101325 Pa).
### `void calcAlt(float p0)`
Calculates altitude from `p0` value.

***
## Output data
### `temperature`
Temperature in Celsium degrees.
### `pressure`
Pressure in Pascals.
### `altitude`
Altitude in meters.

***

