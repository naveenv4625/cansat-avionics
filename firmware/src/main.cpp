#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"
#include <Adafruit_LSM6DSOX.h>

Adafruit_BMP3XX bmp;
Adafruit_LSM6DSOX sox;

const float SEA_LEVEL_HPA = 1011.85;   // from weather report, 2026-10-02
const float GROUND_ELEV_M = 273.0;     // from topo map


void setup() {
  Serial.begin(115200);
  delay(2000);

  // BMP388 setup (pressure/temp sensor)
  if(!bmp.begin_I2C()) {
    Serial.print("Could not connect to BMP388");
    while (1) {
      delay(10);
    }
  }
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_2X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);

  // LSM6DSOX setup (Accelerometer/Gyroscope)
  if(!sox.begin_I2C()) {
    Serial.print("Could not connect to LSM6DSOX");
    while (1) {
      delay(10);
    }
  }
  sox.setAccelRange(LSM6DS_ACCEL_RANGE_16_G);
  sox.setGyroRange(LSM6DS_GYRO_RANGE_2000_DPS);
  sox.setAccelDataRate(LSM6DS_RATE_6_66K_HZ);
  sox.setGyroDataRate(LSM6DS_RATE_6_66K_HZ); 
}

void loop() {
  Serial.print(millis());
  
  // BMP readings
  if(!bmp.performReading()) {
    Serial.println("Failed to read BMP388 data");
    delay(500);
    return;
  }

  Serial.print(bmp.temperature);
  Serial.print(",");

  Serial.print(bmp.pressure / 100.0);
  Serial.print(",");

  Serial.print(bmp.readAltitude(SEA_LEVEL_HPA)-GROUND_ELEV_M);
  Serial.print(",");

  //SOX readings
  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t temp;
  if(!sox.getEvent(&accel, &gyro, &temp)) {
    Serial.println("Failed to read LSM6DSOX data");
    delay(500);
    return;
  }

  Serial.print(temp.temperature);
  Serial.print(",");

  Serial.print(accel.acceleration.x);
  Serial.print(","); Serial.print(accel.acceleration.y);
  Serial.print(","); Serial.print(accel.acceleration.z);
  Serial.print(",");

  Serial.print(gyro.gyro.x);
  Serial.print(","); Serial.print(gyro.gyro.y);
  Serial.print(","); Serial.print(gyro.gyro.z);

  Serial.println();
  delayMicroseconds(10000);
}