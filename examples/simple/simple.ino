#include "BMP388_Barsotion.h"

BMP388_t bmp;

void setup() {
  Serial.begin(115200);
  
  bmp.init(0x77);

  uint8_t buf[6];
  while (1)
  {
    bmp.readRegister(BMP388_PRESS_XLSB, buf, 6);
    bmp.calcTemp(buf+3);
    bmp.calcPres(buf+0);
    Serial.print(bmp.temperature);
    Serial.print("\t");
    Serial.println(bmp.pressure);
    delay(500);
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
