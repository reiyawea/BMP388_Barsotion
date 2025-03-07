#include "BMP388_Barsotion.h"

/* The BMP388 simple example */

/* !
 * Note that the pressure calculations depends on temperature data.
 * When the weather is cold and temperature measurements are done in warm room,
 * the pressure values will be bigger, because room temperature is high while
 * the pressure is as at cold outdoor.
 *
 * This process can affect the altitude measurements. Use calcAlt(p0) function
 * for setting the base altitude if you are not outdoor in winter.
 */

#define SDA_PIN     17
#define SCL_PIN     18

BMP388_t bmp;

void setup() {
  Serial.begin(115200);
  
  /* Address 0x76 or 0x77 */
//  bmp.init(0x77);
  bmp.init(0x77, SDA_PIN, SCL_PIN);

  uint8_t buf[6];
  while (1)
  {
    bmp.readRegister(BMP388_PRESS_XLSB, buf, 6);
    bmp.calcTemp(buf+3);
    bmp.calcPres(buf+0);
    bmp.calcAlt();
    Serial.print(bmp.temperature);
    Serial.print("\t");
    Serial.print(bmp.pressure);
    Serial.print("\t");
    Serial.println(bmp.altitude);
    delay(500);
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
