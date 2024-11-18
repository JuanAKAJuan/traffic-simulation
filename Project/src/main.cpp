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

Signal NS_Signal = Green;  ///< North-South signal.
Signal WE_Signal = Red;  ///< West-East signal.

Vector3 carPosition = { 3, 0, 45 }; ///< Car position with initial value.
Vector3 localCarSpeed = { 0, 0, 0 }; ///< Car speed in car's local space.
Vector3 worldCarSpeed; ///< Car speed in world space.

float carDirection = 180;  ///< Car direction
string carHeading = "N"; ///< String for car heading
float carSpeed = 0.0f;
float maxSpeed = 0.5f;
float acceleration = 0.01f;
float deceleration = 0.005f;
float turnSpeed = 2.0f;

Vector3	localCameraOffset = { 0, 0, -6 };  ///< Third person camera offset in the car's local space.
Vector3 worldCameraOffset;  ///< Third person camera offset in world space.

int winWidth; ///< Width of OpenGL window
int winHeight; ///< Height of OpenGL window
int sWidth; ///< Width of the small viewport
int sHeight; ///< Height of the small viewport

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
	if (key == GLUT_KEY_LEFT)
	{
		// Update car direction
		carDirection += turnSpeed;
		if (carDirection >= 360)
			carDirection -= 360;

		// Update the third person camera offset in the world frame.

		// Compute the car heading.

	}
	if (key == GLUT_KEY_RIGHT)
	{
		carDirection -= turnSpeed;
		if (carDirection < 0)
			carDirection += 360;
	}
	if (key == GLUT_KEY_UP)
	{
		carSpeed = min(maxSpeed, carSpeed + acceleration);
	}

	if (key == GLUT_KEY_DOWN)
	{
		carSpeed = max(-maxSpeed / 2, carSpeed - acceleration);
	}

	glutPostRedisplay();
}

/**
* Function to draw the entire scene.
*/
void drawScene()
{
	// Draw terrain
	glCallList(terrainID);

	glEnable(GL_LIGHTING);

	// North-East (NS_Signal)
	glPushMatrix();
	glTranslatef(10, 0, -10.5);
	glScalef(1/3.28/12, 1/3.28/12, 1/3.28/12);
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
	glScalef(1/3.28/12, 1/3.28/12, 1/3.28/12);
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
	glScalef(1/3.28/12, 1/3.28/12, 1/3.28/12);
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
	glScalef(1/3.28/12, 1/3.28/12, 1/3.28/12);
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
		glScalef(1/3.28/12, 1/3.28/12, 1/3.28/12);

		// Draw the car using display list
		glCallList(carID);
	glPopMatrix();
}

/// Initialization.
/// Set up lighting, generate display lists for the surveillance camera, 
/// car, and terrain.
void init()
{
	glClearColor(0.5, 0.5, 1.0, 1);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Note that the light is defined in the eye or camera frame.
	GLfloat light_position[] = {0.0, 0.0, 0.0, 1.0};

	GLfloat ambient[] = {0.3, 0.3, 0.3, 1};
	GLfloat diffuse[] = {1.0, 1.0, 1.0, 1};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1};

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
	* White dashed lines for lane seperation (Extra Credit)
	*************************************************/
	glColor3f(1, 1, 1);

	float dashLength = 3.0f;
	float gapLength = 9.0f;
	float cycleLength = dashLength + gapLength;

	// Draw dashed lines for North-South road
	for (float z = -1000; z < 1000; z += cycleLength) {
		// Skip the intersection area
		if (z > -10 && z < 10) continue;

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
		if (x > -10 && x < 10) continue;

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
/// Displays 4 viewports.  For for each viewport, set up position and size, projection, 
/// and camera (ModelView matrix).
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_LIGHTING);
	glColor3f(1, 1, 1);

	glWindowPos2i(10, winHeight - 20);

	// Generate head-up display (HUD)
	stringstream ss;
	ss << "Speed: " << fixed << setprecision(2) << carSpeed;
	ss << " Direction: " << static_cast<int>(carDirection);
	ss << " Position: (" << carPosition.x << ", " << carPosition.z << ")";

	printString(ss.str());
	glEnable(GL_LIGHTING);



	/*************************************************
	* Main View
	*************************************************/
	glViewport(0, 0, winWidth, winHeight - sHeight - 50);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30, (float) winWidth / (winHeight - sHeight - 50), 1, 1000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Update the code here for the correct third person camera that moves with the car.
	gluLookAt(carPosition.x, carPosition.y + 2, carPosition.z + 5, carPosition.x, carPosition.y + 1.5,
		carPosition.z, 0, 1, 0);

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

	gluLookAt(cameraRightSideX, cameraHeight, cameraRightSideZ,
		      carPosition.x, carPosition.y + 2, carPosition.z,
			  0, 1, 0);

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
	gluLookAt(0, 100, 0,   // Camera position (high up, looking down)
			  carPosition.x, carPosition.y, carPosition.z,	   // Look at center
			  0, 0, -1);   // Up vector (pointed south, since we're looking down)

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

	gluLookAt(cameraLeftSideX, cameraHeight, cameraLeftSideZ,
		      carPosition.x, carPosition.y + 2, carPosition.z,
			  0, 1, 0);

	drawScene();

	glutSwapBuffers();
	glFlush();
}

/**
* Handle regular key presses, and for P2, "r" for reset, "b" for break, and escape for quit.
*/
void keyboard(unsigned char key, int x, int y)
{
	switch(key)
	{
	case 'r':
	case 'R':
		carPosition = { 3, 0, 45 };
		carDirection = 180;
		carSpeed = 0;
		break;
	case 'b':
	case 'B':
		carSpeed = 0;
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

	carPosition.x += carSpeed * sin(angleRadians);
	carPosition.z += carSpeed * cos(angleRadians);

	if (carSpeed > 0) {
		carSpeed = max(0.0f, carSpeed - deceleration);
	}
	else if (carSpeed < 0) {
		carSpeed = min(0.0f, carSpeed + deceleration);
	}

	// State machine for the traffic signals using three variables: NS_Signal, WE_Signal, and counter.
}

/// Set the interval between updates.
/// \param miliseconds is the number of miliseconds passed before this function is called.  It is the third
/// parameter of glutTimerFunc().
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
	glutTimerFunc(0, timer, updateInterval);
	glutMainLoop();

	system("pause");
}