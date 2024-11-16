/// \file
/// This is the main file for the project Traffic Simulation.
/// \author Juan Mireles
/// \version 1.0
/// \date 11/16/2024
///

#include <iostream>
#include <sstream>
#include <string>

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
}

/// Callback function for special keys.
/// \param key ASCII code of the key pressed.
/// \param x X coordinate of the mouse cursor when the key is pressed.
/// \param y Y coordinate of the mouse cursor when the key is pressed.
void specialKey(int key, int x, int y)
{
	if (key == GLUT_KEY_LEFT)
	{
		// Update car direction

		// Update the third person camera offset in the world frame.

		// Compute the car heading.

	}
	if (key == GLUT_KEY_RIGHT)
	{
		// Handle the right turns.
	}
	if (key == GLUT_KEY_UP)
	{
		// acceleration
	}

	if (key == GLUT_KEY_DOWN)
	{
		// deceleration
	}
}

/// Function to draw the entire scene.
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

	// South-East (WE_Signal)

	// North-West (WE_Signal)

	// Draw the car.
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

	// Generate the display list for terrain, including road and grass.
	terrainID = glGenLists(1);
	glNewList(terrainID, GL_COMPILE);
	glDisable(GL_LIGHTING);

	// Grass
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

	// Roads
	glBegin(GL_QUADS);
		glColor3f(0.2, 0.2, 0.2);

		glVertex3f(-10, 0, 1000);
		glVertex3f(10, 0, 1000);
		glVertex3f(10, 0, -1000);
		glVertex3f(-10, 0, -1000);

		glVertex3f(-1000, 0, 10);
		glVertex3f(1000, 0, 10);
		glVertex3f(1000, 0, -10);
		glVertex3f(-1000, 0, -10);
	glEnd();

	// Yellow line
	glBegin(GL_POLYGON);
		glColor3f(1, 1, 0);
		glVertex3f(-0.1, 0.05, 1000);
		glVertex3f(0.1, 0.05, 1000);
		glVertex3f(0.1, 0.05, -1000);
		glVertex3f(-0.1, 0.05, -1000);
	glEnd();

	glEndList();
}

/// Display callback.
/// Displays 4 viewports.  For for each viewport, set up position and size, projection, 
/// and camera (ModelView matrix).
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Generate head-up display (HUD)
	stringstream ss;

	// Setup viewport, projection, and camera for the main view.
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

	// Setup viewport, projection, and camera for the South-East camera and draw the scene again.


	// Setup the viewport, projection, camera for the top view and draw the scene again.

	
	// Setup viewport, projection, camera for the South-West camera and draw the scene again.

	glutSwapBuffers();
	glFlush();
}

/// Keyboard callback
/// Handle regular key presses, and for P2, "r" for reset, "b" for break, and escape for quit.
void keyboard(unsigned char key, int x, int y)
{
	switch(key)
	{
	case 'r':
		// Add code for reset
		break;
	case 'b':
		// Add code for breaking.
		break;
	case 27:
		exit(0);
		break;
	}

	glutPostRedisplay();
}

/// Updates the dynamic objects.
/// Update the car position and traffic signals.
void update()
{
	// Update car position.

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