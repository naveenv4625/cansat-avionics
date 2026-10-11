#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM6DSOX.h>

Adafruit_LSM6DSOX sox;


void setup() {
  Serial.begin(115200);
  delay(6000);

  const size_t samples = 500;
  float sum1 = 0, sum2 = 0, sum3 = 0,
  sum4 = 0, sum5 = 0, sum6 = 0;

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

  // Z axis values
  Serial.println("Place the sensor facing up (+Z up) on a flat surface press enter when ready");
    while (Serial.available() == 0) {}            
    delay(50);                                     
    while (Serial.available() > 0) Serial.read();
    delay(1500);
        Serial.println("Measuring... Do not move");
            for(size_t i = 0; i < samples; ++i) {
            sensors_event_t accel;
            sensors_event_t gyro;
            sensors_event_t temp;
            if(!sox.getEvent(&accel, &gyro, &temp)) {
                Serial.println("Failed to read LSM6DSOX data");
                delay(500);
                return;
            }

            sum1 += accel.acceleration.x;
            sum2 += accel.acceleration.y;
            sum3 += accel.acceleration.z;

            
            delay(10);    
        }
    Serial.println("Board facing up values:");
    Serial.print(sum1 / samples);
    Serial.print(","); Serial.print(sum2 / samples);
    Serial.print(","); Serial.println(sum3 / samples);

    Serial.println("Place the sensor facing down (-Z up) on a flat surface press enter when ready");
    while (Serial.available() == 0) {}            
    delay(50);                                     
    while (Serial.available() > 0) Serial.read();
    delay(1500);
        Serial.println("Measuring... Do not move");
        for(size_t i = 0; i < samples; ++i) {
            sensors_event_t accel;
            sensors_event_t gyro;
            sensors_event_t temp;
            if(!sox.getEvent(&accel, &gyro, &temp)) {
                Serial.println("Failed to read LSM6DSOX data");
                delay(500);
                return;
            }

            sum4 += accel.acceleration.x;
            sum5 += accel.acceleration.y;
            sum6 += accel.acceleration.z;

            
            delay(10);    
        }
        Serial.println("Board facing down values:");
        Serial.print(sum4 / samples);
        Serial.print(","); Serial.print(sum5 / samples);
        Serial.print(","); Serial.println(sum6 / samples);
    
    float zUp   = sum3 / samples;
    float zDown = sum6 / samples;
    Serial.print("Z offset: "); Serial.println((zUp + zDown) / 2, 4);
    Serial.print("Z scale: ");  Serial.println((zUp - zDown) / (2 * 9.80665), 4);
        sum1 = 0, sum2 = 0, sum3 = 0,
        sum4 = 0, sum5 = 0, sum6 = 0;
    // X axis test
    Serial.println("Place the X-axis facing up (+X up) on a flat surface press enter when ready");
  while (Serial.available() == 0) {}            
    delay(50);                                     
    while (Serial.available() > 0) Serial.read();
    delay(1500);
    Serial.println("Measuring... Do not move");
        for(size_t i = 0; i < samples; ++i) {
            sensors_event_t accel;
            sensors_event_t gyro;
            sensors_event_t temp;
            if(!sox.getEvent(&accel, &gyro, &temp)) {
                Serial.println("Failed to read LSM6DSOX data");
                delay(500);
                return;
            }

            sum1 += accel.acceleration.x;
            sum2 += accel.acceleration.y;
            sum3 += accel.acceleration.z;

            
            delay(10);    
        }
    Serial.println("X facing up values:");
    Serial.print(sum1 / samples);
    Serial.print(","); Serial.print(sum2 / samples);
    Serial.print(","); Serial.println(sum3 / samples);

    Serial.println("Place the X axis facing down (-X up) on a flat surface press enter when ready");
    while (Serial.available() == 0) {}            
    delay(50);                                     
    while (Serial.available() > 0) Serial.read();
    delay(1500);
        Serial.println("Measuring... Do not move");
        for(size_t i = 0; i < samples; ++i) {
            sensors_event_t accel;
            sensors_event_t gyro;
            sensors_event_t temp;
            if(!sox.getEvent(&accel, &gyro, &temp)) {
                Serial.println("Failed to read LSM6DSOX data");
                delay(500);
                return;
            }

            sum4 += accel.acceleration.x;
            sum5 += accel.acceleration.y;
            sum6 += accel.acceleration.z;

            
            delay(10);    
        }
        Serial.println("X facing down values:");
        Serial.print(sum4 / samples);
        Serial.print(","); Serial.print(sum5 / samples);
        Serial.print(","); Serial.println(sum6 / samples);

    float xUp   = sum1 / samples;
    float xDown = sum4 / samples;
    Serial.print("X offset: "); Serial.println((xUp + xDown) / 2, 4);
    Serial.print("X scale: ");  Serial.println((xUp - xDown) / (2 * 9.80665), 4);
    sum1 = 0, sum2 = 0, sum3 = 0,
    sum4 = 0, sum5 = 0, sum6 = 0;

    Serial.println("Place the Y axis facing up (+Y up) on a flat surface press enter when ready");
  while (Serial.available() == 0) {}            
    delay(50);                                     
    while (Serial.available() > 0) Serial.read();
    delay(1500);
    Serial.println("Measuring... Do not move");
        for(size_t i = 0; i < samples; ++i) {
            sensors_event_t accel;
            sensors_event_t gyro;
            sensors_event_t temp;
            if(!sox.getEvent(&accel, &gyro, &temp)) {
                Serial.println("Failed to read LSM6DSOX data");
                delay(500);
                return;
            }

            sum1 += accel.acceleration.x;
            sum2 += accel.acceleration.y;
            sum3 += accel.acceleration.z;

            
            delay(10);    
        }
    Serial.println("Y facing up values:");
    Serial.print(sum1 / samples);
    Serial.print(","); Serial.print(sum2 / samples);
    Serial.print(","); Serial.println(sum3 / samples);

    Serial.println("Place the Y axis facing down (-Y up) on a flat surface press enter when ready");
    while (Serial.available() == 0) {}            
    delay(50);                                     
    while (Serial.available() > 0) Serial.read();
    delay(1500);
        Serial.println("Measuring... Do not move");
        for(size_t i = 0; i < samples; ++i) {
            sensors_event_t accel;
            sensors_event_t gyro;
            sensors_event_t temp;
            if(!sox.getEvent(&accel, &gyro, &temp)) {
                Serial.println("Failed to read LSM6DSOX data");
                delay(500);
                return;
            }

            sum4 += accel.acceleration.x;
            sum5 += accel.acceleration.y;
            sum6 += accel.acceleration.z;

            
            delay(10);    
        }
        Serial.println("Y facing down values:");
        Serial.print(sum4 / samples);
        Serial.print(","); Serial.print(sum5 / samples);
        Serial.print(","); Serial.println(sum6 / samples);

    float yUp   = sum2 / samples;
    float yDown = sum5 / samples;
    Serial.print("Y offset: "); Serial.println((yUp + yDown) / 2, 4);
    Serial.print("Y scale: ");  Serial.println((yUp - yDown) / (2 * 9.80665), 4);
}

void loop() {
}