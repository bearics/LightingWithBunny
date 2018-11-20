#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <gl/glut.h>
#include <math.h>
#include <iostream>

#define PI 3.14159265

static float viewer[3];

/**
	Draw Coodrination system.
*/
void DrawCoordinationSystem(float length)
{
	glBegin(GL_LINES);
	{
		// x-axis
		glColor3f(1, 0, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(length, 0, 0);
		// y-axis
		glColor3f(0, 1, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(0, length, 0);
		// z-axis
		glColor3f(0, 0, 1);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, length);
	}
	glEnd();
}

void RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the window

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(viewer[0], viewer[1], viewer[2], 0, 0, 0, 0, 1, 0);

	// Draw cooddination system
	DrawCoordinationSystem(20);


	// Draw object



	glutSwapBuffers();
	glFlush();
}

void init(void)
{
	viewer[0] = 15;
	viewer[1] = 15;
	viewer[2] = 15;
}

void SetupRC(void)
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);	// set clear color
	glEnable(GL_DEPTH_TEST);
}

void ChangeSize(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// calculate aspect ratio of the window
	gluPerspective(45.f, (GLfloat)w / h, 0.1f, 1000.f);

	//set modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

}

void ResetDisplay()
{

}

void Keyboard(unsigned char key, int x, int y)
{

	RenderScene();
}

void main(int argc, char* argv[])
{
	init();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(600, 600);
	glutCreateWindow("Torus");
	
	glutReshapeFunc(ChangeSize);
	glutDisplayFunc(RenderScene);
	glutKeyboardFunc(Keyboard);
	SetupRC();	// Init Function
	glutMainLoop();
}