#include <Arduino.h>
#include <Wire.h>

void setup() {
  Wire.begin();
  Serial.begin(115200);
  delay(3000);
  Wire.beginTransmission(0x77);
  Wire.write(0x00);
  Wire.endTransmission(false);
  Wire.requestFrom(0x77, 1);
  Serial.print("Chip id: 0x");
  Serial.println(Wire.read(), HEX);
}

void loop() {
}
