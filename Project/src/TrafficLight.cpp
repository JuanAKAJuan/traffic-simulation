#include "TrafficLight.h"


TrafficLight::TrafficLight(void)
{
}


TrafficLight::~TrafficLight(void)
{
}

void TrafficLight::setMaterials()
{
	// In this function, you are supposed to assign values to the variables redOn, redOff,
	// yellowOn, yellowOff, greenOn, greenOff.

}

void TrafficLight::setSignal(Signal signal)
{
	// You are supposed to assign the materials used in the ObjModel class based on
	// values of the input signal.
}

void TrafficLight::ReadFile(string fileName)
{
	ObjModel::ReadFile(fileName);
	setMaterials();
}