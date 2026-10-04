#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"
#include <Adafruit_LSM6DSOX.h>

Adafruit_BMP3XX bmp;
Adafruit_LSM6DSOX sox;

float calibrated_hPa = 0;

const float accelX = 0, accelY = 0, accelZ = 0;
float gyroX = 0, gyroY = 0, gyroZ = 0;

void calibrateBMP388() {
  const size_t samples = 500;
  double sumP = 0;

  Serial.println("# Calibrating pressure sensor, do not move");

  // warm up loop
  for (int i = 0; i < 50; ++i) {
  bmp.performReading();
  }
  
  for(size_t i = 0; i < samples; ++i) {
    if(!bmp.performReading()) {
      Serial.println("Failed to read BMP388 data");
      while (1) {
        delay(10);
      }
    }
    sumP += bmp.pressure / 100;
    delay(25);
  }

  calibrated_hPa = sumP / samples;

  Serial.print("# Calibrated pressure = ");
  Serial.println(calibrated_hPa, 3);
}

void calibrateLSM6DSOX() {
  const size_t samples = 500;
  float sumX = 0, sumY = 0, sumZ = 0;

  Serial.println("# Calibrating gyro, do not move");


  for(size_t i = 0; i < samples; ++i) {
    sensors_event_t accel;
    sensors_event_t gyro;
    sensors_event_t temp;
    if(!sox.getEvent(&accel, &gyro, &temp)) {
      Serial.println("Failed to read LSM6DSOX data");
      while (1) {
        delay(10);
      }
    }
    sumX += gyro.gyro.x;
    sumY += gyro.gyro.y;
    sumZ += gyro.gyro.z;
    delay(10);
  }

  gyroX = sumX / samples;
  gyroY = sumY / samples;
  gyroZ = sumZ / samples;

  Serial.print("# Calibrated Gyro X,Y,Z = ");
  Serial.print(gyroX, 4);
  Serial.print(","); Serial.print(gyroY, 4);
  Serial.print(","); Serial.println(gyroZ, 4);
}

void setup() {
  Serial.begin(115200);
  delay(4000);

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
  calibrateBMP388();

  // LSM6DSOX setup (Accelerometer/Gyroscope)
  if(!sox.begin_I2C()) {
    Serial.print("Could not connect to LSM6DSOX");
    while (1) {
      delay(10);
    }
  }
  sox.setAccelRange(LSM6DS_ACCEL_RANGE_16_G);
  sox.setGyroRange(LSM6DS_GYRO_RANGE_2000_DPS);
  sox.setAccelDataRate(LSM6DS_RATE_104_HZ);
  sox.setGyroDataRate(LSM6DS_RATE_104_HZ);
  calibrateLSM6DSOX(); 

  // header line 
  Serial.println("time_ms,bmp_temp_C,pressure_hPa,alt_m,imu_temp_C,ax_ms2,ay_ms2,az_ms2,gx_rads,gy_rads,gz_rads");
}

void loop() {
  Serial.print(millis());
  Serial.print(",");
  
  // BMP readings
  if(!bmp.performReading()) {
    Serial.println("Failed to read BMP388 data");
    delay(500);
    return;
  }

  float altitude = 44330.0 * (1.0 - pow((bmp.pressure / 100.0) / calibrated_hPa, 0.1903));

  Serial.print(bmp.temperature);
  Serial.print(",");

  Serial.print(bmp.pressure / 100.0);
  Serial.print(",");

  Serial.print(altitude);
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

  Serial.print(gyro.gyro.x-gyroX);
  Serial.print(","); Serial.print(gyro.gyro.y-gyroY);
  Serial.print(","); Serial.print(gyro.gyro.z-gyroZ);

  Serial.println();
  delay(10);
}