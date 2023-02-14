#include "DaisyDuino.h"
#include "descrete.sensor.h"

DescreteSensor sensor;

void setup() {
  Serial.begin(9600);

  sensor.initialize();

  sensor.set_on_touch([]{ Serial.println("One shot touched!"); }, DescreteSensor::Target::OneShot);
  sensor.set_mode(DescreteSensorPad::Mode::Toggle, DescreteSensor::Target::PlayStop);
}

void loop() {
  sensor.process();
  if (sensor.is_on(DescreteSensor::Target::PlayStop)) Serial.println("play");
  if (sensor.is_on(DescreteSensor::Target::Note)) Serial.println("Laaaa");

  delay(100);  
}