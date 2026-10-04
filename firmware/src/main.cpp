#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"

Adafruit_BMP3XX bmp;

const float SEA_LEVEL_HPA = 1011.85;   // from weather report, 2026-10-02
const float GROUND_ELEV_M = 273.0;     // from topo map


void setup() {
  Serial.begin(115200);
  delay(2000);
  if(!bmp.begin_I2C()) {
    Serial.print("Could not connect to BMP388");
    return;
  }
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_2X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);
}

void loop() {
  if(!bmp.performReading()) {
    Serial.println("Failed to read data");
    delay(500);
    return;
  }
  Serial.print("Temperature = ");
  Serial.print(bmp.temperature);
  Serial.println(" *C");

  Serial.print("Pressure = ");
  Serial.print(bmp.pressure / 100.0);
  Serial.println(" hPa");

  Serial.print("Approx. Altitude = ");
  Serial.print(bmp.readAltitude(SEA_LEVEL_HPA)-GROUND_ELEV_M);
  Serial.println(" m");

  Serial.println();
  delay(2000);
}
