#include "daisy_seed.h"
#include "descrete.sensor.h"

using namespace daisy;

DaisySeed hw;
DescreteSensor sensor;


int main(void) {
	hw.Init();
	hw.StartLog();

	sensor.initialize();

  	sensor.set_on_touch([]{ hw.PrintLine("One shot!\n"); }, DescreteSensor::Target::OneShot);
  	sensor.set_mode(DescreteSensorPad::Mode::Toggle, DescreteSensor::Target::PlayStop);

	while(1) {
		sensor.process();
  		if (sensor.is_on(DescreteSensor::Target::PlayStop)) hw.PrintLine("play\n");
  		if (sensor.is_on(DescreteSensor::Target::Note)) hw.PrintLine("Laaa\n");
		System::Delay(100);
	}
}
