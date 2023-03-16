/******************************************************************************
 *
 * Animation v1.0 (23/02/2021)
 *
 * This template provides a basic FPS-limited render loop for an animated scene.
 *
 ******************************************************************************/

#include <Windows.h>
#include <freeglut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


 /******************************************************************************
  * Animation & Timing Setup
  ******************************************************************************/

  // Target frame rate (number of Frames Per Second).
#define TARGET_FPS 60				

// Ideal time each frame should be displayed for (in milliseconds).
const unsigned int FRAME_TIME = 1000 / TARGET_FPS;

// Frame time in fractional seconds.
// Note: This is calculated to accurately reflect the truncated integer value of
// FRAME_TIME, which is used for timing, rather than the more accurate fractional
// value we'd get if we simply calculated "FRAME_TIME_SEC = 1.0f / TARGET_FPS".
const float FRAME_TIME_SEC = (1000 / TARGET_FPS) / 1000.0f;

// Time we started preparing the current frame (in milliseconds since GLUT was initialized).
unsigned int frameStartTime = 0;

/******************************************************************************
 * Keyboard Input Handling Setup
 ******************************************************************************/

 // Define all character keys used for input (add any new key definitions here).
 // Note: USE ONLY LOWERCASE CHARACTERS HERE. The keyboard handler provided converts all
 // characters typed by the user to lowercase, so the SHIFT key is ignored.

#define KEY_EXIT			27 // Escape key.

/******************************************************************************
 * GLUT Callback Prototypes
 ******************************************************************************/

void display(void);
void reshape(int width, int h);
void keyPressed(unsigned char key, int x, int y);
void idle(void);

/******************************************************************************
 * Animation-Specific Function Prototypes (add your own here)
 ******************************************************************************/

void main(int argc, char** argv);
void init(void);
void think(void);

void drawSky(void);

void initGround(void);
void drawGround(void);

// snowman helper functions
void drawHead(void);
void drawLegs(void);
void drawArms(void);

void drawSnowman(void);

/******************************************************************************
 * Animation-Specific Setup (Add your own definitions, constants, and globals here)
 ******************************************************************************/

 // GROUND SETUP

#define NUM_GVERTICES 10
 // x vertices constant, randomise y vertices
GLfloat gxVertices[NUM_GVERTICES] = {-1.0, -0.9, -0.8, -0.7, -0.6, -0.1, 0.2, 0.5, 0.7, 1.0};
GLfloat gyVertices[NUM_GVERTICES];

 /******************************************************************************
  * Entry Point (don't put anything except the main function here)
  ******************************************************************************/

void main(int argc, char** argv)
{
	// Initialize the OpenGL window.
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1000, 800);
	glutCreateWindow("2D Snow Scene");

	// Set up the scene.
	init();

	// Disable key repeat (keyPressed or specialKeyPressed will only be called once when a key is first pressed).
	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);

	// Register GLUT callbacks.
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyPressed);
	glutIdleFunc(idle);

	// Record when we started rendering the very first frame (which should happen after we call glutMainLoop).
	frameStartTime = (unsigned int)glutGet(GLUT_ELAPSED_TIME);

	// Enter the main drawing loop (this will never return).
	glutMainLoop();
}

/******************************************************************************
 * GLUT Callbacks (don't add any other functions here)
 ******************************************************************************/

 /*
	 Called when GLUT wants us to (re)draw the current animation frame.

	 Note: This function must not do anything to update the state of our simulated
	 world. Animation (moving or rotating things, responding to keyboard input,
	 etc.) should only be performed within the think() function provided below.
 */
void display(void)
{
	// clear screen
	glClear(GL_COLOR_BUFFER_BIT);

	// enable smooth lines
	glEnable(GL_LINE_SMOOTH);

	drawSky();

	drawGround();

	drawSnowman();

	glutSwapBuffers();
	/*
		TEMPLATE: REPLACE THIS COMMENT WITH YOUR DRAWING CODE

		Separate reusable pieces of drawing code into functions, which you can add
		to the "Animation-Specific Functions" section below.

		Remember to add prototypes for any new functions to the "Animation-Specific
		Function Prototypes" section near the top of this template.
	*/
}

/*
	Called when the OpenGL window has been resized.
*/
void reshape(int width, int h)
{
}

/*
	Called each time a character key (e.g. a letter, number, or symbol) is pressed.
*/
void keyPressed(unsigned char key, int x, int y)
{
	switch (tolower(key)) {
		/*
			TEMPLATE: Add any new character key controls here.

			Rather than using literals (e.g. "d" for diagnostics), create a new KEY_
			definition in the "Keyboard Input Handling Setup" section of this file.
		*/
	case KEY_EXIT:
		exit(0);
		break;
	}
}

/*
	Called by GLUT when it's not rendering a frame.

	Note: We use this to handle animation and timing. You shouldn't need to modify
	this callback at all. Instead, place your animation logic (e.g. moving or rotating
	things) within the think() method provided with this template.
*/
void idle(void)
{
	// Wait until it's time to render the next frame.

	unsigned int frameTimeElapsed = (unsigned int)glutGet(GLUT_ELAPSED_TIME) - frameStartTime;
	if (frameTimeElapsed < FRAME_TIME)
	{
		// This frame took less time to render than the ideal FRAME_TIME: we'll suspend this thread for the remaining time,
		// so we're not taking up the CPU until we need to render another frame.
		unsigned int timeLeft = FRAME_TIME - frameTimeElapsed;
		Sleep(timeLeft);
	}

	// Begin processing the next frame.

	frameStartTime = glutGet(GLUT_ELAPSED_TIME); // Record when we started work on the new frame.

	think(); // Update our simulated world before the next call to display().

	glutPostRedisplay(); // Tell OpenGL there's a new frame ready to be drawn.
}

/******************************************************************************
 * Animation-Specific Functions (Add your own functions at the end of this section)
 ******************************************************************************/

 /*
	 Initialise OpenGL and set up our scene before we begin the render loop.
 */
void init(void)
{
	// seed random
	srand(time(0));

	//TEMPORARY set bg color to be black for easier viewing of ground
	glClearColor(0, 0, 0, 1.0);

	// set window mode to 2d orthographic and set coordinate system
	gluOrtho2D(-1.0, 1.0, -1.0, 1.0);

	// initialise ground
	initGround();
}

/*
	Advance our animation by FRAME_TIME milliseconds.

	Note: Our template's GLUT idle() callback calls this once before each new
	frame is drawn, EXCEPT the very first frame drawn after our application
	starts. Any setup required before the first frame is drawn should be placed
	in init().
*/
void think(void)
{
	/*
		TEMPLATE: REPLACE THIS COMMENT WITH YOUR ANIMATION/SIMULATION CODE

		In this function, we update all the variables that control the animated
		parts of our simulated world. For example: if you have a moving box, this is
		where you update its coordinates to make it move. If you have something that
		spins around, here's where you update its angle.

		NOTHING CAN BE DRAWN IN HERE: you can only update the variables that control
		how everything will be drawn later in display().

		How much do we move or rotate things? Because we use a fixed frame rate, we
		assume there's always FRAME_TIME milliseconds between drawing each frame. So,
		every time think() is called, we need to work out how far things should have
		moved, rotated, or otherwise changed in that period of time.

		Movement example:
		* Let's assume a distance of 1.0 GL units is 1 metre.
		* Let's assume we want something to move 2 metres per second on the x axis
		* Each frame, we'd need to update its position like this:
			x += 2 * (FRAME_TIME / 1000.0f)
		* Note that we have to convert FRAME_TIME to seconds. We can skip this by
		  using a constant defined earlier in this template:
			x += 2 * FRAME_TIME_SEC;

		Rotation example:
		* Let's assume we want something to do one complete 360-degree rotation every
		  second (i.e. 60 Revolutions Per Minute, or RPM).
		* Each frame, we'd need to update our object's angle like this (we'll use the
		  FRAME_TIME_SEC constant as per the example above):
			a += 360 * FRAME_TIME_SEC;

		This works for any type of "per second" change: just multiply the amount you'd
		want to move in a full second by FRAME_TIME_SEC, and add or subtract that
		from whatever variable you're updating.

		You can use this same approach to animate other things like color, opacity,
		brightness of lights, etc.
	*/
}

void drawSky() {
	// draw sky
	glBegin(GL_POLYGON);

	// set color to light blue (194, 250, 252) RGB255
	glColor3f(194.0 / 255.0, 250.0 / 255.0, 252.0 / 255.0);
	// bottom left vertex
	glVertex2f(-1.0, -1.0);
	// bottom right vertex
	glVertex2f(1.0, -1.0);
	
	// set color to dark blue (129, 192, 205) RGB255
	glColor3f(129.0 / 255.0, 192.0 / 255.0, 205.0 / 255.0);
	// top right vertex
	glVertex2f(1.0, 1.0);
	// top left vertex
	glVertex2f(-1.0, 1.0);

	glEnd();
}

void initGround() {

	// randomise y value of top ground vertices
	for (int i = 0; i < NUM_GVERTICES; i++) {
		// if even, make point higher
		if (i % 2 == 0) {
			gyVertices[i] = (rand() % (-30 - (-45) + 1) - 45) * 0.01; // between -0.3 and -0.45
		}
		else {
			gyVertices[i] = (rand() % (-45 - (-50) + 1) - 50) * 0.01; // between -0.45 and -0.5
		}
	}
}

void drawGround(void) {
	// draw ground
	glBegin(GL_QUAD_STRIP);

	for (int i = 0; i < NUM_GVERTICES; i++) {
		// set color to dark green (76, 139, 100) RGB255
		glColor3f(76.0 / 255.0, 139.0 / 255.0, 100.0 / 255.0);
		// bottom vertex
		glVertex2f(gxVertices[i], -1.0);
		// set color to light green (137, 172, 115) RGB255
		glColor3f(137.0 / 255.0, 172.0 / 255.0, 115.0 / 255.0);
		// top vertex
		glVertex2f(gxVertices[i], gyVertices[i]);
	}
	glEnd();
}

void drawSnowman(void) {
	drawLegs();
	drawArms();
	drawHead();
}

void drawArms(void) {
	// set line width
	glLineWidth(22.5f);

	glBegin(GL_LINES);

	// set color to orange (239, 143, 60) RGB255
	glColor3f(239.0 / 255.0, 143.0 / 255.0, 60.0 / 255.0);

	// left arm
	glVertex2f(-0.17, 0.05);
	glVertex2f(-0.3, -0.22);

	// left hand middle part
	glVertex2f(-0.35, -0.2);
	glVertex2f(-0.23, -0.24);

	// left hand left part
	glVertex2f(-0.33, -0.2);
	glVertex2f(-0.38, -0.3);

	// left hand right part
	glVertex2f(-0.24, -0.21);
	glVertex2f(-0.25, -0.34);

	glEnd();

	// set line width
	glLineWidth(29.0f);

	glBegin(GL_LINES);

	// right arm
	glVertex2f(0.17, 0.05);
	glVertex2f(0.3, -0.22);

	glEnd();

	glBegin(GL_POLYGON);

	// right hand
	// top point
	glVertex2f(0.3, -0.18);
	// right top point
	glVertex2f(0.42, -0.22);
	// right bottom point
	glVertex2f(0.44, -0.35);
	// left bottom point
	glVertex2f(0.30, -0.4);
	// left top point
	glVertex2f(0.23, -0.29);

	glEnd();
}

void drawLegs(void) {
	// set line width
	glLineWidth(27.0f);

	glBegin(GL_LINES);

	// set color to orange (239, 143, 60) RGB255
	glColor3f(239.0 / 255.0, 143.0 / 255.0, 60.0 / 255.0);

	// left leg
	glVertex2f(-0.095, 0.0);
	glVertex2f(-0.095, -0.6);

	// right leg
	glVertex2f(0.095, 0.0);
	glVertex2f(0.095, -0.6);

	// left foot
	glVertex2f(-0.069, -0.6);
	glVertex2f(-0.2, -0.61);

	// right foot
	glVertex2f(0.069, -0.6);
	glVertex2f(0.2, -0.61);

	glEnd();
}

void drawHead(void) {
	float radius = 0.2f;

	glBegin(GL_TRIANGLE_FAN);

	// set color to light orange (242, 184, 102)
	glColor3f(242.0 / 255.0, 184.0 / 255.0, 102.0 / 255.0);
	glVertex2f(0.0, 0.1);

	for (int theta = 0; theta <= 360; theta += 10)
	{
		// set color to orange (239, 143, 60) RGB255
		glColor3f(239.0 / 255.0, 143.0 / 255.0, 60.0 / 255.0);
		float angle = (float)(theta * 3.14 / 180.0);
		float x = cos(angle) * radius;
		float y = sin(angle) * radius + 0.1;
		glVertex2f(x, y);
	}
	glEnd();
}
/******************************************************************************/