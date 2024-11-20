#include "TrafficLight.h"

TrafficLight::TrafficLight(void) { }

TrafficLight::~TrafficLight(void) { }

/**
 * Assigns values to the variables redOn, redOff, yellowOn, yellowOff, greenOn,
 * greenOff.
 */
void TrafficLight::setMaterials()
{
    redOn.Ka[0] = 0.5f;
    redOn.Ka[1] = 0.0f;
    redOn.Ka[2] = 0.0f; // Ambient
    redOn.Kd[0] = 1.0f;
    redOn.Kd[1] = 0.0f;
    redOn.Kd[2] = 0.0f; // Diffuse
    redOn.Ks[0] = 1.0f;
    redOn.Ks[1] = 0.0f;
    redOn.Ks[2] = 0.0f; // Specular
    redOn.Ns = 900.0f; // Shininess
    redOn.d = 1.0f; // Opacity

    redOff = redOn;
    redOff.Ka[0] = 0.2f; // Reduced ambient
    redOff.Kd[0] = 0.2f; // Reduced diffuse
    redOff.Ks[0] = 0.2f; // Reduced specular

    yellowOn.Ka[0] = 0.5f;
    yellowOn.Ka[1] = 0.5f;
    yellowOn.Ka[2] = 0.0f;
    yellowOn.Kd[0] = 1.0f;
    yellowOn.Kd[1] = 1.0f;
    yellowOn.Kd[2] = 0.0f;
    yellowOn.Ks[0] = 1.0f;
    yellowOn.Ks[1] = 1.0f;
    yellowOn.Ks[2] = 0.0f;
    yellowOn.Ns = 900.0f;
    yellowOn.d = 1.0f;

    yellowOff = yellowOn;
    yellowOff.Ka[0] = 0.2f;
    yellowOff.Ka[1] = 0.2f;
    yellowOff.Kd[0] = 0.2f;
    yellowOff.Kd[1] = 0.2f;
    yellowOff.Ks[0] = 0.2f;
    yellowOff.Ks[1] = 0.2f;

    greenOn.Ka[0] = 0.0f;
    greenOn.Ka[1] = 0.5f;
    greenOn.Ka[2] = 0.0f;
    greenOn.Kd[0] = 0.0f;
    greenOn.Kd[1] = 1.0f;
    greenOn.Kd[2] = 0.0f;
    greenOn.Ks[0] = 0.0f;
    greenOn.Ks[1] = 1.0f;
    greenOn.Ks[2] = 0.0f;
    greenOn.Ns = 900.0f;
    greenOn.d = 1.0f;

    greenOff = greenOn;
    greenOff.Ka[1] = 0.2f;
    greenOff.Kd[1] = 0.2f;
    greenOff.Ks[1] = 0.2f;
}

/**
 * Assigns the materials used in the ObjModel class based on values of the input
 * signal.
 */
void TrafficLight::setSignal(Signal signal)
{
    auto& mats = materials;

    // Set all signals to off state first
    mats["_Red_"] = redOff;
    mats["_Yellow_"] = yellowOff;
    mats["_Green_"] = greenOff;

    switch (signal) {
    case Red:
        mats["_Red_"] = redOn;
        break;
    case Yellow:
        mats["_Yellow_"] = yellowOn;
        break;
    case Green:
        mats["_Green_"] = greenOn;
        break;
    }
}

void TrafficLight::ReadFile(string fileName)
{
    ObjModel::ReadFile(fileName);
    setMaterials();
}