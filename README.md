# Arduino library for BMP388 barometer

## Methods
### `BMP388_t`
Constructor.
### `void init(uint8_t addr)`
Initializes the sensor.
### `uint8_t getWhoAmI()`
The sensor's ID should be 0x50.
### `void calcTemp(uint8_t *raw)`
Calculates temperature. `raw[0]` should contain the `BMP388_TEMP_XLSB` register value, `raw[1]` - `BMP388_TEMP_LSB`, `raw[2]` - `BMP388_TEMP_MSB`.
### `void calcPres(uint8_t *raw)`
Calculates pressure. `raw[0]` should contain the `BMP388_PRESS_XLSB` register value, `raw[1]` - `BMP388_PRESS_LSB`, `raw[2]` - `BMP388_PRESS_MSB`.
### `void calcAlt()`
Calculates altitude from sea level value.
### `void calcAlt(float p0)`
Calculates altitude from `p0` value.

## Output data
### `temperature`
Temperature in Celsium degrees.
### `pressure`
Pressure in Pascals.
### `altitude`
Altitude in meters.

## TODO
- Add ODR, OVS, IIR filt coef setters descriptions
