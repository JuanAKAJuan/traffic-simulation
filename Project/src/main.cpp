/// \file
/// This is the main file for the project Traffic Simulation.
/// \author Juan Mireles
/// \version 1.0
/// \date 11/16/2024
///

#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include <algorithm>

#include <cmath>

#include <gl/glew.h>
#include <gl/glut.h>

#include "ObjModel.h"
#include "TrafficLight.h"
#include "utility.h"

using namespace std;

int counter = 0; ///< Counter for the traffic signal.  Only one is needed.
int updateInterval = 20; ///< Update interval for the update function in miliseconds.

ObjModel car;
ObjModel surveillanceCamera;
TrafficLight trafficLight;

int carID; ///< Display List ID for car
int surveillanceCameraID; ///< Display list ID for surveillance camera
int terrainID; ///< Display list ID for terrain

Signal NS_Signal = Green; ///< North-South signal.
Signal WE_Signal = Red; ///< West-East signal.

Vector3 carPosition = { 3, 0, 45 }; ///< Car position with initial value.
Vector3 localCarSpeed = { 0, 0, 0 }; ///< Car speed in car's local space.
Vector3 worldCarSpeed; ///< Car speed in world space.

float carDirection = 180; ///< Car direction
string carHeading = "N"; ///< String for car heading
float carSpeed = 0.0f;
float maxSpeed = 0.5f;
float acceleration = 0.01f;
float deceleration = 0.005f;

float targetDirection = 180.0f; // The direction the car is turning towards
float turnInterpolation = 0.0f; // Current turn progress
float turnAcceleration = 0.2f;
float turnDeceleration = 0.1f;
float maxTurnSpeed = 6.0f;
float minTurnSpeed = 2.0f;
float turnSpeedMultiplier = 1.0f;

bool isMovingForward = false;
bool isMovingBackward = false;
bool isTurningLeft = false;
bool isTurningRight = false;

bool treeId;
struct TreeInstance {
    float x, z;
    float rotation;
    float scale;
};
vector<TreeInstance> trees;

Vector3 localCameraOffset = { 0, 0, -6 }; ///< Third person camera offset in the car's local space.
Vector3 worldCameraOffset = localCameraOffset; ///< Third person camera offset in world space.

int winWidth; ///< Width of OpenGL window
int winHeight; ///< Height of OpenGL window
int sWidth; ///< Width of the small viewport
int sHeight; ///< Height of the small viewport

void drawTree();
void initTrees();
bool isNearTrafficLight(float x, float z);
bool isValidTreePosition(float x, float z);
void drawTrees();


/// Update the small viewports' size automatically.
/// \param w Width of the OpenGL window
/// \param h Height of the OpenGL window
void reshape(int w, int h)
{
    winWidth = w, winHeight = h;

    // Update sWidth and sHeight here.
    sWidth = winWidth / 4;
    sHeight = winHeight / 4;
}

/**
 * Callback function for special keys.
 * \param key ASCII code of the key pressed.
 * \param x X coordinate of the mouse cursor when the key is pressed.
 * \param y Y coordinate of the mouse cursor when the key is pressed.
 */
void specialKey(int key, int x, int y)
{
    switch (key) {
    case GLUT_KEY_LEFT:
        isTurningLeft = true;
        break;
    case GLUT_KEY_RIGHT:
        isTurningRight = true;
        break;
    case GLUT_KEY_UP:
        isMovingForward = true;
        break;
    case GLUT_KEY_DOWN:
        isMovingBackward = true;
        break;
    }

    worldCameraOffset = computeRotatedVector(localCameraOffset, carDirection);
    glutPostRedisplay();
}

void specialKeyUp(int key, int x, int y)
{
    switch (key) {
    case GLUT_KEY_LEFT:
        isTurningLeft = false;
        break;
    case GLUT_KEY_RIGHT:
        isTurningRight = false;
        break;
    case GLUT_KEY_UP:
        isMovingForward = false;
        break;
    case GLUT_KEY_DOWN:
        isMovingBackward = false;
        break;
    }

}

/**
 * Function to draw the entire scene.
 */
void drawScene()
{
    // Draw terrain
    glCallList(terrainID);

    drawTrees();

    glEnable(GL_LIGHTING);

    // North-East (NS_Signal)
    glPushMatrix();
    glTranslatef(10, 0, -10.5);
    glScalef(1 / 3.28 / 12, 1 / 3.28 / 12, 1 / 3.28 / 12);
    trafficLight.setSignal(NS_Signal);
    trafficLight.Draw();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(10, 0, -10);
    glRotatef(-45, 0, 1, 0);
    glCallList(surveillanceCameraID);
    glPopMatrix();

    // South-West (NS_Signal)
    glPushMatrix();
    glTranslatef(-10, 0, 10.5);
    glRotatef(180, 0, 1, 0);
    glScalef(1 / 3.28 / 12, 1 / 3.28 / 12, 1 / 3.28 / 12);
    trafficLight.setSignal(NS_Signal);
    trafficLight.Draw();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-10, 0, 10);
    glRotatef(135, 0, 1, 0);
    glCallList(surveillanceCameraID);
    glPopMatrix();

    // South-East (WE_Signal)
    glPushMatrix();
    glTranslatef(10, 0, 10.5);
    glRotatef(-90, 0, 1, 0);
    glScalef(1 / 3.28 / 12, 1 / 3.28 / 12, 1 / 3.28 / 12);
    trafficLight.setSignal(WE_Signal);
    trafficLight.Draw();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(10, 0, 10);
    glRotatef(-135, 0, 1, 0);
    glCallList(surveillanceCameraID);
    glPopMatrix();

    // North-West (WE_Signal)
    glPushMatrix();
    glTranslatef(-10, 0, -10.5);
    glRotatef(90, 0, 1, 0);
    glScalef(1 / 3.28 / 12, 1 / 3.28 / 12, 1 / 3.28 / 12);
    trafficLight.setSignal(WE_Signal);
    trafficLight.Draw();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-10, 0, -10);
    glRotatef(45, 0, 1, 0);
    glCallList(surveillanceCameraID);
    glPopMatrix();

    // Draw the car.
    glPushMatrix();

    // Position the car
    glTranslatef(carPosition.x, carPosition.y, carPosition.z);

    // Rotate the car based on its direction
    glRotatef(carDirection, 0, 1, 0); // Rotate around the Y axis

    // Scale the car
    glScalef(1 / 3.28 / 12, 1 / 3.28 / 12, 1 / 3.28 / 12);

    glEnable(GL_LIGHTING);

    // Draw the car using display list
    glCallList(carID);
    glPopMatrix();
}

/// Initialization.
/// Set up lighting, generate display lists for the surveillance camera,
/// car, and terrain.
void init() {
    initTrees();

    glClearColor(0.5, 0.5, 1.0, 1);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Note that the light is defined in the eye or camera frame.
    GLfloat light_position[] = { 0.0, 0.0, 0.0, 1.0 };

    GLfloat ambient[] = { 0.3, 0.3, 0.3, 1 };
    GLfloat diffuse[] = { 1.0, 1.0, 1.0, 1 };
    GLfloat specular[] = { 1.0, 1.0, 1.0, 1 };

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

    glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
    glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);

    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // Generate display list for the surveillance camera.
    surveillanceCameraID = glGenLists(1);

    glNewList(surveillanceCameraID, GL_COMPILE);
    surveillanceCamera.Draw();
    glEndList();

    // Generate display list for the car.
    carID = glGenLists(1);
    glNewList(carID, GL_COMPILE);
    car.Draw();
    glEndList();

    // Generate the display list for terrain, including road and grass.
    terrainID = glGenLists(1);
    glNewList(terrainID, GL_COMPILE);
    glDisable(GL_LIGHTING);

    /*************************************************
     * Grass
     *************************************************/
    glColor3f(0, 0.7, 0);
    glBegin(GL_QUADS);
    glVertex3f(-1000, 0, 1000);
    glVertex3f(-10, 0, 1000);
    glVertex3f(-10, 0, 10);
    glVertex3f(-1000, 0, 10);

    glVertex3f(10, 0, 1000);
    glVertex3f(1000, 0, 1000);
    glVertex3f(1000, 0, 10);
    glVertex3f(10, 0, 10);

    glVertex3f(10, 0, -10);
    glVertex3f(1000, 0, -10);
    glVertex3f(1000, 0, -1000);
    glVertex3f(10, 0, -1000);

    glVertex3f(-1000, 0, -10);
    glVertex3f(-10, 0, -10);
    glVertex3f(-10, 0, -1000);
    glVertex3f(-1000, 0, -1000);
    glEnd();

    /*************************************************
     * Roads
     *************************************************/
    glBegin(GL_QUADS);
    glColor3f(0.2, 0.2, 0.2);

    // North-South road
    glVertex3f(-10, 0, 1000);
    glVertex3f(10, 0, 1000);
    glVertex3f(10, 0, -1000);
    glVertex3f(-10, 0, -1000);

    // East-West road
    glVertex3f(-1000, 0, 10);
    glVertex3f(1000, 0, 10);
    glVertex3f(1000, 0, -10);
    glVertex3f(-1000, 0, -10);
    glEnd();

    /*************************************************
     * Yellow centerlines
     *************************************************/
    glColor3f(1, 1, 0);

    // North section centerline
    glBegin(GL_QUADS);
    glVertex3f(-0.1, 0.05, 1000); // Start from far North
    glVertex3f(0.1, 0.05, 1000);
    glVertex3f(0.1, 0.05, 10); // Stop at intersection
    glVertex3f(-0.1, 0.05, 10);
    glEnd();

    // South section centerline
    glBegin(GL_QUADS);
    glVertex3f(-0.1, 0.05, -10); // Start from intersection
    glVertex3f(0.1, 0.05, -10);
    glVertex3f(0.1, 0.05, -1000); // Go to far south
    glVertex3f(-0.1, 0.05, -1000);
    glEnd();

    // West section centerline
    glBegin(GL_QUADS);
    glVertex3f(-1000, 0.05, 0.1); // Start from far west
    glVertex3f(-10, 0.05, 0.1); // Stop at intersection
    glVertex3f(-10, 0.05, -0.1);
    glVertex3f(-1000, 0.05, -0.1);
    glEnd();

    // East section centerline
    glBegin(GL_QUADS);
    glVertex3f(10, 0.05, 0.1); // Start from intersection
    glVertex3f(1000, 0.05, 0.1); // Go to far east
    glVertex3f(1000, 0.05, -0.1);
    glVertex3f(10, 0.05, -0.1);
    glEnd();

    /*************************************************
     * White dashed lines for lane separation (Extra Credit)
     *************************************************/
    glColor3f(1, 1, 1);

    float dashLength = 3.0f;
    float gapLength = 9.0f;
    float cycleLength = dashLength + gapLength;

    // Draw dashed lines for North-South road
    for (float z = -1000; z < 1000; z += cycleLength) {
        // Skip the intersection area
        if (z > -10 && z < 10)
            continue;

        // Left lane divider
        glBegin(GL_QUADS);
        glVertex3f(-5.1, 0.05, z);
        glVertex3f(-4.9, 0.05, z);
        glVertex3f(-4.9, 0.05, z + dashLength);
        glVertex3f(-5.1, 0.05, z + dashLength);
        glEnd();

        // Right lane divider
        glBegin(GL_QUADS);
        glVertex3f(4.9, 0.05, z);
        glVertex3f(5.1, 0.05, z);
        glVertex3f(5.1, 0.05, z + dashLength);
        glVertex3f(4.9, 0.05, z + dashLength);
        glEnd();
    }

    // Draw dashed lines for East-West road
    for (float x = -1000; x < 1000; x += cycleLength) {
        // Skip the intersection area
        if (x > -10 && x < 10)
            continue;

        // Upper lane divider
        glBegin(GL_QUADS);
        glVertex3f(x, 0.05, 5.1);
        glVertex3f(x + dashLength, 0.05, 5.1);
        glVertex3f(x + dashLength, 0.05, 4.9);
        glVertex3f(x, 0.05, 4.9);
        glEnd();

        // Lower lane divider
        glBegin(GL_QUADS);
        glVertex3f(x, 0.05, -4.9);
        glVertex3f(x + dashLength, 0.05, -4.9);
        glVertex3f(x + dashLength, 0.05, -5.1);
        glVertex3f(x, 0.05, -5.1);
        glEnd();
    }

    glEndList();
}

/// Display callback.
/// Displays 4 viewports.  For for each viewport, set up position and size,
/// projection, and camera (ModelView matrix).
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Display HUD information
    glDisable(GL_LIGHTING);
    glColor3f(1, 1, 1);
    glWindowPos2i(10, winHeight - 20);

    // Generate head-up display (HUD)
    stringstream ss;
    ss << "Speed: " << fixed << setprecision(2) << carSpeed;
    ss << " Direction: " << static_cast<int>(carDirection);
    ss << " Heading: (" << carHeading;
    ss << " Position: (" << fixed << setprecision(1) << carPosition.x << ", "
       << carPosition.z << ")";

    printString(ss.str());
    glEnable(GL_LIGHTING);

    /*************************************************
     * Main View (Third Person Camera)
     *************************************************/
    glViewport(0, 0, winWidth, winHeight - sHeight - 50);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, (float)winWidth / (winHeight - sHeight - 50), 1, 1000);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Calculate camera position using the rotated offset
    Vector3 cameraPosition = {
        carPosition.x + worldCameraOffset.x,
        carPosition.y + worldCameraOffset.y + 2.0f, // Height offset
        carPosition.z + worldCameraOffset.z
    };

    Vector3 lookAtPoint = { carPosition.x, carPosition.y + 1.0f, carPosition.z };

    // Update the code here for the correct third person camera that moves with
    // the car.
    gluLookAt(
        cameraPosition.x, cameraPosition.y, cameraPosition.z,
        lookAtPoint.x, lookAtPoint.y, lookAtPoint.z, 
        0, 1, 0
    );

    drawScene();

    /*************************************************
     * Right Side (South-East) Angled View
     *************************************************/
    glViewport(winWidth - sWidth, winHeight - sHeight, sWidth, sHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, static_cast<float>(sWidth) / sHeight, 1, 1000);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float cameraDistanceFromCar = 30.0f;
    float cameraHeight = 5.0f;
    float cameraRightSideAngle = -45.0f;

    float cameraRightSideX = carPosition.x + cameraDistanceFromCar * sin(cameraRightSideAngle * M_PI / 180.0f);
    float cameraRightSideZ = carPosition.z + cameraDistanceFromCar * cos(cameraRightSideAngle * M_PI / 180.0f);

    gluLookAt(
        cameraRightSideX, cameraHeight, cameraRightSideZ,
        carPosition.x, carPosition.y + 2, carPosition.z,
        0, 1, 0
    );

    drawScene();

    /*************************************************
     * Top Down View
     *************************************************/
    int centerX = (winWidth - sWidth) / 2;

    glViewport(centerX, winHeight - sHeight, sWidth, sHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-50, 50, -50, 50, -1000, 1000);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(
        carPosition.x, 100, carPosition.z, // Camera position (high up, looking down)
        carPosition.x, 0, carPosition.z, // Look at center
        0, 0, -1  // Up vector (pointed south, since we're looking down)
    );

    drawScene();

    /*************************************************
     * Left Side (South-West) Angled View
     *************************************************/
    glViewport(0, winHeight - sHeight, sWidth, sHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, static_cast<float>(sWidth) / sHeight, 1, 1000);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float cameraLeftSideAngle = 45.0f;

    float cameraLeftSideX = carPosition.x + cameraDistanceFromCar * sin(cameraLeftSideAngle * M_PI / 180.0f);
    float cameraLeftSideZ = carPosition.z + cameraDistanceFromCar * cos(cameraLeftSideAngle * M_PI / 180.0f);

    gluLookAt(cameraLeftSideX, cameraHeight, cameraLeftSideZ, carPosition.x,
        carPosition.y + 2, carPosition.z, 0, 1, 0);

    drawScene();

    glutSwapBuffers();
    glFlush();
}

/**
 * Handle regular key presses, and for P2, "r" for reset, "b" for break, and
 * escape for quit.
 */
void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case 'r':
    case 'R':
        carPosition = { 3, 0, 45 };
        carDirection = 180;
        carSpeed = 0;
        turnInterpolation = 0;
        isMovingForward = false;
        isMovingBackward = false;
        isTurningLeft = false;
        isTurningRight = false;
        break;
    case 'b':
    case 'B':
        carSpeed = 0;
        isMovingForward = false;
        isMovingBackward = false;
        break;
    case 27:
        exit(0);
        break;
    }

    glutPostRedisplay();
}

/**
 * Updates the dynamic objects, the car position and traffic signals.
 */
void update()
{
    float angleRadians = carDirection * M_PI / 180.0f;

    if (isMovingForward) {
        carSpeed = min(maxSpeed, carSpeed + acceleration);
    } else if (isMovingBackward) {
        carSpeed = max(-maxSpeed / 2, carSpeed - acceleration);
    } else {
		// Natural speed decay (simulating friction/drag)
		if (carSpeed > 0) {
			carSpeed = max(0.0f, carSpeed - deceleration);
		} else if (carSpeed < 0) {
			carSpeed = min(0.0f, carSpeed + deceleration);
		}
    }

    // Calculate dynamic turn rate based on speed
    float speedFactor = abs(carSpeed) / maxSpeed;
    float currentTurnSpeed = minTurnSpeed + (maxTurnSpeed - minTurnSpeed) * (1.0f - speedFactor);
    currentTurnSpeed *= turnSpeedMultiplier;

    // Smooth turning logic
    if (isTurningLeft || isTurningRight) {
        // Gradually increase turn interpolation
        turnInterpolation = min(1.0f, turnInterpolation + turnAcceleration);
    } else {
        // Gradually increase turn interpolation
        turnInterpolation = max(0.0f, turnInterpolation - turnAcceleration);
    }

    // Apply smooth turning
    if (turnInterpolation > 0) {
        float turnAmount = currentTurnSpeed * turnInterpolation;

        if (isTurningLeft) {
            carDirection += turnAmount;
        }
        if (isTurningRight) {
            carDirection -= turnAmount;
        }


        while (carDirection >= 360)
            carDirection -= 360;
        while (carDirection < 0)
            carDirection += 360;
    }

    // sin for x (east-west) and cos for z (north-south)
    carPosition.x += carSpeed * sin(angleRadians);
    carPosition.z += carSpeed * cos(angleRadians);

    // Update car heading based on direction
    if (carDirection >= 315 || carDirection < 45) {
        carHeading = "N";
    } else if (carDirection >= 45 && carDirection < 135) {
        carHeading = "E";
    } else if (carDirection >= 135 && carDirection < 225) {
        carHeading = "S";
    } else {
        carHeading = "W";
    }

    worldCameraOffset = computeRotatedVector(localCameraOffset, carDirection);

    // State machine for the traffic signals using three variables: NS_Signal,
    // WE_Signal, and counter.
    counter += updateInterval; // Add elapsed time in milliseconds

    const int greenTime = 5000; // 5 seconds
    const int yellowTime = 1000; // 1 second
    const int redTime = 6000; // 6 seconds
    const int totalCycle = greenTime + yellowTime + redTime;

    if (counter >= totalCycle) {
        counter = 0;
    }

    // North-South signal states
    if (counter < greenTime) {
        NS_Signal = Green;
        WE_Signal = Red;
    } else if (counter < greenTime + yellowTime) {
        NS_Signal = Yellow;
        WE_Signal = Red;
    } else {
        NS_Signal = Red;
        WE_Signal = Green;

        if (counter > totalCycle - yellowTime) {
            WE_Signal = Yellow;
        }
    }
}

/**
* Draw a basic tree
*/
void drawTree() {
    // Tree trunk
    glColor3f(0.55f, 0.27f, 0.07f);
    GLUquadricObj* trunk = gluNewQuadric();
    glPushMatrix();
    glRotatef(-90, 1, 0, 0); // Make the trunk vertical
    gluCylinder(trunk, 0.5, 0.5, 4, 10, 1);
    glPopMatrix();
    gluDeleteQuadric(trunk);
    
    // Tree foliage
    glColor3f(0.13f, 0.55f, 0.13f);
    GLUquadricObj* leaves = gluNewQuadric();

    // Bottom cone
    glPushMatrix();
    glTranslatef(0, 3, 0);
    glRotatef(-90, 1, 0, 0);
    gluCylinder(leaves, 3, 0, 4, 10, 1);
    glPopMatrix();

    // Middle cone
    glPushMatrix();
    glTranslatef(0, 5, 0);
    glRotatef(-90, 1, 0, 0);
    gluCylinder(leaves, 2.5, 0, 3.5, 10, 1);
    glPopMatrix();

    // Top cone
    glPushMatrix();
    glTranslatef(0, 7, 0);
    glRotatef(-90, 1, 0, 0);
    gluCylinder(leaves, 2, 0, 3, 10, 1);
    glPopMatrix();

    gluDeleteQuadric(leaves);
}

void initTrees() {
    treeId = glGenLists(1);
    glNewList(treeId, GL_COMPILE);
    drawTree();
    glEndList();

    trees.clear();

    const float TREE_SPACING = 50.0f;

    for (int quadrant = 0; quadrant < 4; quadrant++) {
        float xStart = (quadrant & 1) ? 20.0f : -1000.0f;
        float xEnd = (quadrant & 1) ? 1000.0f : -20.0f;
        float zStart = (quadrant & 2) ? 20.0f : -1000.0f;
        float zEnd = (quadrant & 2) ? 1000.0f : -20.0f;

        for (float x = xStart; x < xEnd; x += TREE_SPACING) {
            for (float z = zStart; z < zEnd; z += TREE_SPACING) {
                float offsetX = (rand() % 20) - 10.0f;
                float offsetZ = (rand() % 20) - 10.0f;
                float treeX = x + offsetX;
                float treeZ = z + offsetZ;

                if (isValidTreePosition(treeX, treeZ)) {
                    TreeInstance tree;
                    tree.x = treeX;
                    tree.z = treeZ;
                    tree.rotation = rand() % 360;
                    tree.scale = 0.8f + (rand() % 4) * 0.1f;
                    trees.push_back(tree);
                }
            }
        }
    }
}

/**
* Checks to see if a tree position is too close to a traffic light
*/
bool isNearTrafficLight(float x, float z) {
    struct TrafficLight {
        float x;
        float z;
    };

    const TrafficLight trafficLights[] = {
        { 10.0f, -10.5f }, // North-East
        { -10.0f, -10.5f }, // North-West
        { 10.0f, 10.5f }, // South-East
        { -10.0f, 10.5f }, // South-West
    };

    const float MIN_DISTANCE = 20.0f;

    // Check distance to each traffic light
    for (int i = 0; i < 4; i++) {
        float lightX = trafficLights[i].x;
        float lightZ = trafficLights[i].z;
        
        float distance = sqrt(pow(x - lightX, 2) + pow(z - lightZ, 2));
        if (distance < MIN_DISTANCE)
            return true;
    }

    return false;
}

/**
* Checks to see if a tree can be placed in a location
*/
bool isValidTreePosition(float x, float z) {
    const float ROAD_CLEARANCE = 15.0f;
    if (abs(x) < ROAD_CLEARANCE || abs(z) < ROAD_CLEARANCE) {
        return false;
    }

    if (isNearTrafficLight(x, z)) {
        return false;
    }

    return true;
}

void drawTrees() {
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    // Sort trees back to front
    sort(trees.begin(), trees.end(),
        [](const TreeInstance& a, const TreeInstance& b) {
            return (a.z * a.z + a.x * a.x) > (b.z * b.z + b.x * b.x);
        });

    for (const auto& tree : trees) {
        glPushMatrix();
        glTranslatef(tree.x, 0, tree.z);
        glRotatef(tree.rotation, 0, 1, 0);
        glScalef(tree.scale, tree.scale, tree.scale);
        glCallList(treeId);
        glPopMatrix();
    }

    glEnable(GL_LIGHTING);
}

/// Set the interval between updates.
/// \param miliseconds is the number of miliseconds passed before this function
/// is called.  It is the third parameter of glutTimerFunc().
void timer(int miliseconds)
{
    update();
    glutTimerFunc(updateInterval, timer, updateInterval);
    glutPostRedisplay();
}

/// Main function
/// GLUT initialization, load 3D models, and register GLUT callbacks.
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    winWidth = 1300, winHeight = 800;
    glutInitWindowSize(winWidth, winHeight);
    glutCreateWindow("Traffic Simulation");
    glewInit();

    // Load the 3D models.
    trafficLight.ReadFile("Models/TrafficLight.obj");
    car.ReadFile("Models/Honda_S2000_inch.obj");
    surveillanceCamera.ReadFile("Models/camera.obj");

    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKey);
    glutSpecialUpFunc(specialKeyUp);
    glutTimerFunc(0, timer, updateInterval);
    glutMainLoop();

    system("pause");
}