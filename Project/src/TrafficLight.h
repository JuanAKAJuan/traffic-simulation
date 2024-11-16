#pragma once
#include "objmodel.h"

/// \struct Signal
/// Structure for storing traffic signal types
enum Signal {Green, Yellow, Red};

/// \class TrafficLight
/// A Class for a traffic light
class TrafficLight :
	public ObjModel
{
public:
	TrafficLight(void);
	~TrafficLight(void);

	/// Read the OBJ and MTL files.  Initialize the material values.
	/// \param fileName Name of the OBJ file
	virtual void ReadFile(string fileName);

	/// Initialize values of redOn, redOff, yellowOn, yellowOff, greenOn, and greenOff.
	void setMaterials();

	/// Set the current signal of the traffic light.
	/// \param signal The new signal of the traffic light.
	void setSignal(Signal signal);

private:
	/// The material values for each signal on and off.  Note by default, private members are not shown
	/// in Doxygen, but it can be enabled by checking EXTRACT_PRIVATE tag.
	Material redOn, redOff, yellowOn, yellowOff, greenOn, greenOff;
};

