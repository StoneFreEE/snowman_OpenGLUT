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
#define KEY_PARTICLE_ONOFF  's' // s key
#define KEY_LASER		    'l' // l key


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

// LASER FUNCTIONS
void drawLaser();
void updateLaser();

// PARTICLE SYSTEM FUNCTIONS
void initParticleSystem();
void updateParticleSystem(float deltaTime);
void drawParticleSystem();
int findUnusedParticle();
void spawnParticle(int index);

void displayDiagnostics(void);

// background drawing functions
void drawSky(void);

void initGround(void);
void drawGround(void);

void drawPlatform(void);

// snowman helper functions
void drawHead(void);
void drawLegs(void);
void drawArms(void);
void drawDetails(void);

void drawCircle(GLfloat x, GLfloat y, float radius);

void drawSnowman(void);

/******************************************************************************
 * Animation-Specific Setup (Add your own definitions, constants, and globals here)
 ******************************************************************************/

 // PARTICLES
#define MAX_PARTICLES 100

int lastUsedParticle = -1;
int particleSystemActive = 0; // particle system initally inactive
int particleCounter = 0;

float particleSpawnTimer = 0.0f;
float spawnDelay = 0.3f;

typedef struct {
	float x;
	float y;
} Position2;

typedef struct {
	Position2 position; //x y location of particle
	float size; // GL point size
	float dy;   //velocity
	boolean active;
	float life; // total time particle has been alive
	float alpha; // transparency
	GLfloat colour[3];
}  Particle_t;

GLfloat colours[5][3] = { {168.0f / 255.0f, 100 / 255.0f,253 / 255.0f}, // purple
						  {41 / 255.0f, 205 / 255.0f, 255 / 255.0f}, // blue
						  {120 / 255.0f,255 / 255.0f,68 / 255.0f}, // green
						  {255 / 255.0f, 113 / 255.0f,141 / 255.0f}, // red
						  {253 / 255.0f, 255 / 255.0f,106 / 255.0f} // yellow
};

float pAlpha = 1.0f;
float deltaTime = 0;
Particle_t particleSystem[MAX_PARTICLES];

// LASER SETUP

// laser rotation angle
GLfloat laserRotation = 0.0f;
int laserActive = 0; // laser initally inactive
GLfloat laserVertices[4][2] = { {-0.05f, 0.01f}, {0.0f, 0.0f}, {1.5f, 0.5f}, {1.5f, 1.5f} }; // bottom left, bottom right, top right, top left vertex coordinates


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

	drawParticleSystem();

	drawPlatform();

	drawSnowman();

	displayDiagnostics();

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

	case KEY_PARTICLE_ONOFF:
		if (particleSystemActive == 1) {
			particleSystemActive = 0;
		}
		else {
			particleSystemActive = 1;
			spawnDelay = 0.3f;
		}
		break;
	case KEY_LASER:
		if (laserActive) {
			laserActive = 0;
		}
		else {
			laserActive = 1;
		}
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
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glutInitDisplayMode(GLUT_RGBA);

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
	updateParticleSystem(FRAME_TIME_SEC);
	updateLaser();
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

// LASER FUNCTION
void drawLaser() {
	// save the current matrix
	glPushMatrix();

	// rotate the laser around the z-axis by the current rotation angle
	glTranslatef(0.0f, 0.1f, 0.0f);
	glRotatef(laserRotation, 0.0f, 0.0f, 1.0f);

	// set color to red and alpha to 0.4 
	glColor4f(1.0f, 0.0f, 0.0f, 0.4f);
	glBegin(GL_POLYGON);
	for (int i = 0; i < 4; i++) {
		glVertex2f(laserVertices[i][0], laserVertices[i][1]);
	}
	glEnd();
	// restore the previous matrix
	glPopMatrix();

	updateLaser();
}

void updateLaser() {
	laserRotation += 90 * FRAME_TIME_SEC;
}

// PARTICLE SYSTEM FUNCTIONS

int findUnusedParticle() {
	for (int i = lastUsedParticle + 1; i < MAX_PARTICLES; i++) {
		if (particleSystem[i].active == 0) {
			lastUsedParticle = i;
			return i;
		}
	}

	for (int i = 0; i < lastUsedParticle; i++) {
		if (particleSystem[i].active == 0) {
			lastUsedParticle = i;
			return i;
		}
	}

	return 0; // All particles are in use, override the first one
}

void spawnParticle(int index) {
	int colourindex = rand() % 5;

	particleSystem[index].position.x = (double)rand() / ((double)RAND_MAX / 2) - 1;
	particleSystem[index].position.y = 1.0f;
	particleSystem[index].size = ((double)rand() / RAND_MAX) * (10.0 - 5.0) + 5.0;
	particleSystem[index].dy = ((double)rand() / RAND_MAX * 0.4 + 0.1) * particleSystem[index].size / 10; // random num between 0.1-0.4 * particle size / 10
	particleSystem[index].life = 0.0f;
	particleSystem[index].alpha = ((double)rand() / RAND_MAX * 1.0 + 0.5); // random num between 0.5-1.0
	for (int i = 0; i < 3; i++) {
		particleSystem[index].colour[i] = colours[colourindex][i];
	}
	particleSystem[index].active = 1;
	particleCounter++;
}

void updateParticleSystem(float deltaTime) {

	int unusedParticle = findUnusedParticle();

	if (particleSystemActive) {
		particleSpawnTimer += deltaTime;
		int i = lastUsedParticle;

		do {
			i = (i + 1) % MAX_PARTICLES;
			if (!particleSystem[i].active && particleSpawnTimer >= spawnDelay) {
				spawnParticle(i);
				spawnDelay -= deltaTime * 0.1f; 
				particleSpawnTimer = 0.0f;
				if (spawnDelay < 0.0f) {
					spawnDelay = 0.0f; // Ensure particle spawn rate doesn't become negative
				}

				lastUsedParticle = i;
				break;
			}
		} while (i != lastUsedParticle);
	}

	// update particle location transparency etc
	for (int i = 0; i < MAX_PARTICLES; i++) {
		Particle_t* p = &particleSystem[i];
		if (p->active) {
			p->position.y -= p->dy * deltaTime;
			p->life += deltaTime;
			if (p->position.y <= -1.0f) {
				p->active = 0;
				particleCounter--;
			}
		}
	}
}

void drawParticleSystem() {
	for (int i = 0; i < MAX_PARTICLES; i++) {
		if (particleSystem[i].active == 1) {
			glPointSize(particleSystem[i].size);
			glBegin(GL_POINTS);
			glColor4f(particleSystem[i].colour[0], particleSystem[i].colour[1], particleSystem[i].colour[2], particleSystem[i].alpha); // gradually fade out particle
			glVertex2f(particleSystem[i].position.x, particleSystem[i].position.y);
			glEnd();
		}
	}
}


// DIAGNOSTICS FUNCTION

void displayDiagnostics(void){
	char text[256]; // Allocate a buffer to hold the formatted string
	snprintf(text, sizeof(text), "Diagnostics\n particles %d of %d\nScene controls:\n %c: Toggle confetti\n %c: Toggle laser\n ESC: exit", particleCounter, MAX_PARTICLES, KEY_PARTICLE_ONOFF, KEY_LASER);

	glColor3f(0.0f, 0.0f, 0.0f);
	// Set the position for the text
	glRasterPos2f(-0.99f, 0.95f);

	// Render the text 
	glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)text);
}


// BACKGROUND DRAWING FUNCTIONS

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

void drawPlatform(void) {
	// draw bottom
	glBegin(GL_POLYGON);
	// set colour to dark dull green
	glColor3f(65.0 / 255.0, 97.0 / 255.0, 65.0 / 255.0);
	//bottom left
	glVertex2f(-0.4, -1.0f);
	// bottom right
	glVertex2f(0.4, -1.0f);
	// top right
	glVertex2f(0.4, -0.7f);
	// top left
	glVertex2f(-0.4, -0.7f);

	glEnd();

	// draw top
	glBegin(GL_POLYGON);
	// set colour to dull green
	glColor3f(93.0 / 255.0, 129.0 / 255.0, 93.0 / 255.0);
	//bottom left
	glVertex2f(-0.4, -0.7f);
	// bottom right
	glVertex2f(0.4, -0.7f);
	// top right
	glVertex2f(0.25, -0.5f);
	// top left
	glVertex2f(-0.25, -0.5f);
	
	glEnd();

}

// SNOWMAN DRAWING FUNCTIONS

void drawSnowman(void) {
	drawLegs();
	drawArms();
	drawHead();
	drawDetails();
}

void drawDetails(void) {
	// set color to black and draw eyes
	glColor3f(0.0, 0.0, 0.0);

	// right eye small circle guy
	drawCircle(-0.18, 0.25, 0.015);
	// left eye small circle guy
	drawCircle(-0.215, 0.2, 0.015);

	// outer big circle eye
	drawCircle(0.0, 0.1, 0.1);

	// right hand left eye
	drawCircle(0.3, -0.3, 0.015);
	// right hand right eye
	drawCircle(0.375, -0.27, 0.015);

	// set color to white
	glColor3f(1.0, 1.0, 1.0);
	// white big circle eye
	drawCircle(0.0, 0.1, 0.065);
	if (laserActive) {
		drawLaser();
	}

	// set color to black 
	glColor3f(0.0, 0.0, 0.0);
	// black inner big circle eye
	drawCircle(0.0, 0.1, 0.05);

	// set line width
	glLineWidth(21.5f);

	// draw bottom of top hat
	glBegin(GL_LINES);

	// bottom left
	glVertex2f(-0.1, 0.32);
	// bottom right
	glVertex2f(0.1, 0.32);

	glEnd();

	// draw top of top hat
	glBegin(GL_POLYGON);

	// bottom left
	glVertex2f(-0.07, 0.33);
	// bottom right
	glVertex2f(0.07, 0.34);
	// top right
	glVertex2f(0.08, 0.44);
	// top left
	glVertex2f(-0.08, 0.44);
	glEnd();
}

void drawArms(void) {
	// set line width
	glLineWidth(22.5f);

	glBegin(GL_LINES);

	// left arm
	// set color to red (232, 93, 46) RGB255
	glColor3f(232.0 / 255.0, 93.0 / 255.0, 46.0 / 255.0);
	glVertex2f(-0.17, 0.05);
	// set color to orange (239, 143, 60) RGB255
	glColor3f(239.0 / 255.0, 143.0 / 255.0, 60.0 / 255.0);
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
		// set color to red (232, 93, 46) RGB255
	glColor3f(232.0 / 255.0, 93.0 / 255.0, 46.0 / 255.0);
	glVertex2f(0.17, 0.05);
	// set color to orange (239, 143, 60) RGB255
	glColor3f(239.0 / 255.0, 143.0 / 255.0, 60.0 / 255.0);
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
			// set color to red (232, 93, 46) RGB255
	glColor3f(232.0 / 255.0, 93.0 / 255.0, 46.0 / 255.0);
	glVertex2f(-0.095, 0.0);
	// set color to orange (239, 143, 60) RGB255
	glColor3f(239.0 / 255.0, 143.0 / 255.0, 60.0 / 255.0);
	glVertex2f(-0.095, -0.6);

	// right leg
				// set color to red (232, 93, 46) RGB255
	glColor3f(232.0 / 255.0, 93.0 / 255.0, 46.0 / 255.0);
	glVertex2f(0.095, 0.0);
	// set color to orange (239, 143, 60) RGB255
	glColor3f(239.0 / 255.0, 143.0 / 255.0, 60.0 / 255.0);
	glVertex2f(0.095, -0.6);

	// left foot
	glVertex2f(-0.069, -0.6);
	glVertex2f(-0.2, -0.61);

	// right foot
	glVertex2f(0.069, -0.6);
	glVertex2f(0.2, -0.61);

	glEnd();
}

void drawCircle(GLfloat x, GLfloat y, float radius) {
	glBegin(GL_TRIANGLE_FAN);

	glVertex2f(x, y);

	for (int theta = 0; theta <= 360; theta += 10)
	{
		float angle = (float)(theta * 3.14 / 180.0);
		float finalX = cos(angle) * radius + x;
		float finalY = sin(angle) * radius + y;
		glVertex2f(finalX, finalY);
	}
	glEnd();
}

void drawHead(void) {
	// set color to orange (239, 143, 60) RGB255
	glColor3f(239.0 / 255.0, 143.0 / 255.0, 60.0 / 255.0);
	drawCircle(-0.16, 0.2, 0.08);

	float radius = 0.2f;

	// big circle guy
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