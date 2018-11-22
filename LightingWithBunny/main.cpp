#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <gl/glut.h>
#include <math.h>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

#define PI 3.14159265

static float viewer[3];
static double nPoints;
static int nPolygons;
static double** points;
static int** polygons;


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

/**
	Read Bunny
*/
void ReadBunny(string path)
{
	ifstream fin(path.data());

	if (fin.is_open())
	{
		fin >> nPoints >> nPolygons;

		points = new double*[nPoints];

		for (int n = 0; n < nPoints; n++)
		{	// read coordinates of points 
			points[n] = new double[3];

			fin >> points[n][0] >> points[n][1] >> points[n][2];
		}

		polygons = new int*[nPolygons];

		for (int n = 0; n < nPolygons; n++)
		{	// read sets of points in a polygons
			polygons[n] = new int[3];

			fin >> polygons[n][0] >> polygons[n][1] >> polygons[n][2];
		}

		fin.close();
	}
	else
	{
		cout << "Cannot read file" << endl;
	}

	return;
}

void DrawTorusBunny()
{

	glColor3f(0, 0, 1);

	for (int polygon = 0; polygon < nPolygons; polygon++)
	{	// Draw Bunny as Triangles with line frame
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glBegin(GL_TRIANGLES);
		{
			for (int point = 0; point < 3; point++)
			{
				glVertex3f(
					points[polygons[polygon][point] - 1][0],
					points[polygons[polygon][point] - 1][1],
					points[polygons[polygon][point] - 1][2]);
			}
		}
		glEnd();
	}
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
	DrawTorusBunny();


	// Flush 
	glutSwapBuffers();
	glFlush();
}

void init(void)
{
	viewer[0] = 2;
	viewer[1] = 2;
	viewer[2] = 2;

	// read Bunny
	ReadBunny("bunny_origin.txt");

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