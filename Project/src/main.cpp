/**
* Main implementation file for a traffic simulation system using OpenGL.
*
* @file main.cpp
* @author Juan Mireles
* @version 1.0
* @date 11/16/2024
*/

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

/**
 * Counter for the traffic signal timing system.
 * Used to control the timing of traffic light state changes.
 */
int counter = 0;

/**
 * Update interval for the simulation in milliseconds.
 * Controls how frequently the simulation state is updated.
 */
int updateInterval = 20;

/**
 * Display List identifier for the car model.
 */
int carID;

/**
 * Display List identifier for the surveillance camera model.
 */
int surveillanceCameraID;

/**
 * Display List identifier for the terrain.
 */
int terrainID;

/**
 * Current state of North-South traffic signal.
 */
Signal NS_Signal = Green;

/**
 * Current state of West-East traffic signal.
 */
Signal WE_Signal = Red;

/**
 * Current position of the car in 3D space.
 */
Vector3 carPosition = { 3, 0, 45 };

/**
 * Car velocity in local coordinate space.
 */
Vector3 localCarSpeed = { 0, 0, 0 };

/**
 * Car velocity in world coordinate space.
 */
Vector3 worldCarSpeed;

/**
 * The 3D model object for the car.
 * Represents the vehicle that the user controls in the simulation.
 */
ObjModel car;

/**
 * The 3D model object for the car.
 * Represents the vehicle that the user controls in the simulation.
 */
ObjModel surveillanceCamera;

/**
 * The traffic light object.
 * Represents the traffic signals at intersections, controlling traffic flow.
 * Loaded from "Models/TrafficLight.obj".
 * Used to create multiple instances at different intersections.
 */
TrafficLight trafficLight;

/**
 * Current rotation angle of the car in degrees.
 * Values range from 0 to 360 degrees:
 * <ul>
 *   <li>0/360 degrees points North</li>
 *   <li>90 degrees points East</li>
 *   <li>180 degrees points South</li>
 *   <li>270 degrees points West</li>
 * </ul>
 * Initial value is 180 (facing South).
 */
float carDirection = 180;

/**
 * String representation of the car's current heading direction.
 * Possible values:
 * <ul>
 *   <li>"N" for North (315 to 45 degrees)</li>
 *   <li>"E" for East (45 to 135 degrees)</li>
 *   <li>"S" for South (135 to 225 degrees)</li>
 *   <li>"W" for West (225 to 315 degrees)</li>
 * </ul>
 * Initial value is "N".
 */
string carHeading = "N";

/**
 * Current speed of the car in world units per update.
 * <ul>
 *   <li>Positive values indicate forward movement</li>
 *   <li>Negative values indicate backward movement</li>
 *   <li>Zero indicates the car is stationary</li>
 *   <li>Maximum speed is capped by maxSpeed constant</li>
 *   <li>Reverse speed is capped at maxSpeed/2</li>
 * </ul>
 * Initial value is 0.0f (stationary).
 */
float carSpeed = 0.0f;

/**
 * Maximum forward speed of the car.
 */
const float maxSpeed = 0.5f;

/**
 * Car acceleration rate.
 */
const float acceleration = 0.01f;

/**
 * Car deceleration rate.
 */
const float deceleration = 0.005f;

/**
 * Maximum turning speed.
 */
const float maxTurnSpeed = 6.0f;

/**
 * Minimum turning speed.
 */
const float minTurnSpeed = 2.0f;

/**
 * Target direction (in degrees) that the car is attempting to reach while turning.
 * Similar to carDirection, but represents the final desired angle during a turn:
 * <ul>
 *   <li>0/360 degrees points North</li>
 *   <li>90 degrees points East</li>
 *   <li>180 degrees points South</li>
 *   <li>270 degrees points West</li>
 * </ul>
 * Initial value matches carDirection at 180 degrees.
 */
float targetDirection = 180.0f; // The direction the car is turning towards

/**
 * Progress of the current turning motion, ranging from 0.0 to 1.0.
 * <ul>
 *   <li>0.0 = No turn in progress</li>
 *   <li>0.0-1.0 = Turn in progress</li>
 *   <li>1.0 = Maximum turn rate</li>
 * </ul>
 * Used for smooth turning animation.
 */
float turnInterpolation = 0.0f; // Current turn progress

/**
 * Rate at which turning speed increases when turn input is active.
 * Higher values result in faster turn initiation.
 * Controls how quickly the car responds to turning input.
 */
float turnAcceleration = 0.2f;

/**
 * Rate at which turning speed decreases when turn input is released.
 * Higher values result in faster turn stopping.
 * Controls how quickly the car straightens out after a turn.
 */
float turnDeceleration = 0.1f;

/**
 * Scaling factor for turn speed.
 * <ul>
 *   <li>1.0 = Normal turning speed</li>
 *   <li>>1.0 = Faster turning</li>
 *   <li><1.0 = Slower turning</li>
 * </ul>
 * Can be adjusted to fine-tune turning responsiveness.
 */
float turnSpeedMultiplier = 1.0f;

/**
 * Flag indicating whether the car is currently moving forward.
 * Set to true when up arrow is pressed, false when released.
 * Used in update() to calculate car movement.
 */
bool isMovingForward = false;

/**
 * Flag indicating whether the car is currently moving backward.
 * Set to true when down arrow is pressed, false when released.
 * Used in update() to calculate car movement.
 */
bool isMovingBackward = false;

/**
 * Flag indicating whether the car is currently turning left.
 * Set to true when left arrow is pressed, false when released.
 * Used in update() to calculate car rotation.
 */
bool isTurningLeft = false;

/**
 * Flag indicating whether the car is currently turning right.
 * Set to true when right arrow is pressed, false when released.
 * Used in update() to calculate car rotation.
 */
bool isTurningRight = false;

/**
 * OpenGL display list identifier for the tree model.
 * Used to store compiled tree rendering commands for improved performance.
 * @see drawTree()
 * @see initTrees()
 * @see drawTrees()
 */
bool treeId;

/**
 * Structure defining a tree instance.
 */
struct TreeInstance {
    float x, z;
    float rotation;
    float scale;
};

/**
 * Container storing all tree instances in the scene.
 * Each TreeInstance contains:
 * <ul>
 *   <li>Position (x, z coordinates)</li>
 *   <li>Rotation (in degrees)</li>
 *   <li>Scale factor</li>
 * </ul>
 * @see TreeInstance
 * @see initTrees()
 * @see drawTrees()
 */
vector<TreeInstance> trees;

/**
 * Camera offset relative to the car in the car's local coordinate system.
 * Components represent:
 * <ul>
 *   <li>x (0): No horizontal offset from car's center</li>
 *   <li>y (0): No vertical offset from car's center</li>
 *   <li>z (-6): 6 units behind the car</li>
 * </ul>
 * This offset is transformed based on car's rotation to calculate actual camera position.
 */
Vector3 localCameraOffset = { 0, 0, -6 };

/**
 * Camera offset in world space coordinates.
 * Calculated by rotating localCameraOffset based on car's current direction.
 * Updated whenever the car rotates to maintain correct camera position.
 * Initially set to match localCameraOffset before any rotations.
 */
Vector3 worldCameraOffset = localCameraOffset;

/**
 * Width of the main OpenGL window in pixels.
 * Used for:
 * <ul>
 *   <li>Setting up the main viewport</li>
 *   <li>Calculating aspect ratios</li>
 *   <li>Positioning UI elements</li>
 * </ul>
 * @see reshape()
 */
int winWidth;

/**
 * Height of the main OpenGL window in pixels.
 * Used for:
 * <ul>
 *   <li>Setting up the main viewport</li>
 *   <li>Calculating aspect ratios</li>
 *   <li>Positioning UI elements</li>
 * </ul>
 * @see reshape()
 */
int winHeight;

/**
 * Width of each small viewport in pixels.
 * Calculated as winWidth / 4.
 * Used for the three auxiliary views:
 * <ul>
 *   <li>Right Side (South-East) view</li>
 *   <li>Top Down view</li>
 *   <li>Left Side (South-West) view</li>
 * </ul>
 * @see display()
 */
int sWidth;

/**
 * Height of each small viewport in pixels.
 * Calculated as winHeight / 4.
 * Used for the three auxiliary views:
 * <ul>
 *   <li>Right Side (South-East) view</li>
 *   <li>Top Down view</li>
 *   <li>Left Side (South-West) view</li>
 * </ul>
 * @see display()
 */
int sHeight;

/**
 * Draws a single tree using OpenGL primitives.
 * Creates a tree with trunk and three-level foliage.
 */
void drawTree();

/**
 * Initializes the forest of trees.
 * Randomly places trees in valid positions across the terrain.
 */
void initTrees();

/**
 * Checks if a position is too close to traffic lights.
 *
 * @param x X-coordinate to check
 * @param z Z-coordinate to check
 * @return true if position is too close to traffic lights, false otherwise
 */
bool isNearTrafficLight(float x, float z);

/**
 * Validates a potential tree position.
 *
 * @param x X-coordinate to validate
 * @param z Z-coordinate to validate
 * @return true if position is valid for tree placement, false otherwise
 */
bool isValidTreePosition(float x, float z);

/**
 * Renders all trees in the scene.
 * Sorts trees back-to-front for proper transparency handling.
 */
void drawTrees();

/**
 * Window reshape callback.
 * Updates viewport dimensions and maintains aspect ratio.
 *
 * @param w New window width
 * @param h New window height
 */
void reshape(int w, int h)
{
    winWidth = w, winHeight = h;

    // Update sWidth and sHeight here.
    sWidth = winWidth / 4;
    sHeight = winHeight / 4;
}

/**
 * Handles special key presses (arrow keys).
 * Controls car movement and turning.
 *
 * @param key The key code for the pressed special key
 * @param x Mouse X coordinate when key was pressed
 * @param y Mouse Y coordinate when key was pressed
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

/**
 * Handles special key releases.
 * Stops car movement and turning when keys are released.
 *
 * @param key The key code for the released special key
 * @param x Mouse X coordinate when key was released
 * @param y Mouse Y coordinate when key was released
 */
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
 * Renders the complete scene.
 * Draws terrain, traffic lights, surveillance cameras, trees, and the car.
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

/**
 * Initializes OpenGL settings and creates display lists.
 * Sets up lighting, materials, and generates display lists for models.
 */
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

/**
 * Main display callback function.
 * Renders the scene across four different viewports:
 * <ul>
 *   <li>Main view (Third Person Camera)</li>
 *   <li>Right Side (South-East) Angled View</li>
 *   <li>Top Down View</li>
 *   <li>Left Side (South-West) Angled View</li>
 * </ul>
 */
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
 * Handles regular key presses.
 * Processes commands:
 * <ul>
 *   <li>'r'/'R': Reset car position</li>
 *   <li>'b'/'B': Apply brakes</li>
 *   <li>ESC: Exit program</li>
 * </ul>
 *
 * @param key ASCII code of the pressed key
 * @param x Mouse X coordinate when key was pressed
 * @param y Mouse Y coordinate when key was pressed
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
 * Updates simulation state.
 * Updates car position, traffic signals, and other dynamic elements.
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

/**
 * Timer callback for regular updates.
 * Triggers regular simulation updates and redraws.
 *
 * @param milliseconds Time interval for updates
 */
void timer(int miliseconds)
{
    update();
    glutTimerFunc(updateInterval, timer, updateInterval);
    glutPostRedisplay();
}

/**
 * Main entry point for the traffic simulation.
 * Initializes GLUT, loads 3D models, and starts the main event loop.
 *
 * @param argc Number of command-line arguments
 * @param argv Array of command-line argument strings
 * @return Program exit status
 */
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