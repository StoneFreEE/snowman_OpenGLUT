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

void initParticleSystem();
void updateParticleSystem(float deltaTime);
void drawParticleSystem();
int findunusedParticle();
void spawnParticle();

/******************************************************************************
 * Animation-Specific Setup (Add your own definitions, constants, and globals here)
 ******************************************************************************/
#define MAX_PARTICLES 100

int lastUsedParticle = -1;

int particlesToSpawn = 1; // initial number of particles to spawn per frame
int particlesSpawned = 0; // number of particles spawned this frame


typedef struct {
	float x;
	float y;
} Position2;

typedef struct {
	Position2 position; //x y location of particle
	float size; // GL point size
	float dy;   //velocity
	int active; //0 = is inactive
	float life; // total time particle has been alive
	float alpha; // transparency
}  Particle_t;

int intervalTime = 0;
int releaseInterval = 0;

float pAlpha = 1.0f;
float deltaTime = 0;
Particle_t particleSystem[MAX_PARTICLES];

/******************************************************************************
 * Entry Point (don't put anything except the main function here)
 ******************************************************************************/

void main(int argc, char** argv)
{
	// Initialize the OpenGL window.
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1000, 800);
	glutCreateWindow("Animation");

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

	glClear(GL_COLOR_BUFFER_BIT);
	drawParticleSystem();
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
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glutInitDisplayMode(GLUT_RGBA);

	srand(time(0));

	//TEMPORARY set bg color to be black for easier viewing of ground
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// set window to 2d ortho
	gluOrtho2D(-1.0, 1.0, -1.0, 1.0);


	initParticleSystem();

}

int findUnusedParticle() {
	for (int i = lastUsedParticle; i < MAX_PARTICLES; i++) {
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

	return -1; // All particles are in use, override the first one
}
void spawnParticle(double deltaTime) {
	/*double probability = 0.2; // 20% chance of spawning a particle per frame

	if ((double)rand() / (double)RAND_MAX <= probability) {
		int particleIndex = findUnusedParticle();
		Particle_t* p = &particleSystem[particleIndex];

		p->active = 1;
		p->life = 0.0f;
		p->position.x = (double)rand() / ((double)RAND_MAX / 2) - 1;
		p->position.y = 1.0f;
		p->dy = ((double)rand() / RAND_MAX) * (1.0 - 0.5) + 0.5;
		p->alpha = 1.0f;
		p->size = ((double)rand() / RAND_MAX) * (10.0 - 5.0) + 5.0;
	}*/
	int particleIndex = findUnusedParticle();
	Particle_t* p = &particleSystem[particleIndex];

	p->active = 1;
	p->life = 0.0f;
	p->position.x = (double)rand() / ((double)RAND_MAX / 2) - 1;
	p->position.y = 1.0f;
	p->dy = ((double)rand() / RAND_MAX) * (1.0 - 0.5) + 0.5;
	p->alpha = 1.0f;
	p->size = ((double)rand() / RAND_MAX) * (10.0 - 5.0) + 5.0;
}


void initParticleSystem() {
	for (int i = 0; i < MAX_PARTICLES; i++) {
		particleSystem[i].position.x = (double)rand() / ((double)RAND_MAX / 2) - 1;
		particleSystem[i].position.y = 1.0f;
		particleSystem[i].dy = ((double)rand() / RAND_MAX) + 0.5; // reduce particle velocity to 0.5 - 1.5
		particleSystem[i].life = 0.0f;
		particleSystem[i].alpha = 1.0f;
		particleSystem[i].size = ((double)rand() / RAND_MAX) * (10.0 - 5.0) + 5.0;
		particleSystem[i].active = 0;
	}
}

void updateParticleSystem(float deltaTime) {
	int unusedParticle = findUnusedParticle();

	// spawn particles gradually when the particle system starts
	if (lastUsedParticle == -1) {
		if (particlesSpawned < particlesToSpawn) {
			spawnParticle(unusedParticle);
			particlesSpawned++;
		}
		else {
			lastUsedParticle = unusedParticle;
			particlesSpawned = 0;
			particlesToSpawn++;
			if (particlesToSpawn > 5) {
				particlesToSpawn = 5;
			}
		}
	}
	else {
		spawnParticle(lastUsedParticle);
	}


	// check if unusedParticle is still -1
	if (unusedParticle == -1) {
		lastUsedParticle = 0;
	}


	for (int i = 0; i < MAX_PARTICLES; i++) {
		Particle_t* p = &particleSystem[i];
		if (p->active) {
			p->position.y -= p->dy * deltaTime;
			p->life += deltaTime;
			if (p->position.y <= -1.0f) {
				p->active = 0;
			}
			p->alpha = (p->position.y + 1.0f) / 1.0f;
		}
	}
}



void drawParticleSystem() {
	for (int i = 0; i < MAX_PARTICLES; i++) {
		if (particleSystem[i].active == 1) {
			glPointSize(particleSystem[i].size);
			glBegin(GL_POINTS);
			glColor4f(1.0f, 1.0f, 1.0f, particleSystem[i].alpha); // gradually fade out particle over 3 seconds
			glVertex2f(particleSystem[i].position.x, particleSystem[i].position.y);
			glEnd();
		}
	}
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

/******************************************************************************/