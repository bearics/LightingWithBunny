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
static double** triangleMiddlePoints;
static double** triangleNormalVectors;


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

/**
	Draw Bunny with triangles
*/
void DrawBunny()
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

/**
	Get Triangle's middle points
*/
void GetTriangleMiddlepoints()
{
	triangleMiddlePoints = new double*[nPolygons];

	for (int polygon = 0; polygon < nPolygons; polygon++)
	{	// read coordinates of points 
		triangleMiddlePoints[polygon] = new double[3];

		for (int n = 0; n < 3; n++)
		{	// initialize triangleMiddlePoint
			triangleMiddlePoints[polygon][n] = 0;
		}

		for (int point = 0; point < 3; point++)
		{	// p1, p2, p3 in triangle
			for (int n = 0; n < 3; n++)
			{	// x, y, z
				triangleMiddlePoints[polygon][n] += points[polygons[polygon][point] - 1][n] / 3;
			}
		}

	}
}

/*
	Normalize Vectors
*/
void NormalizeVectors(double* vector, double rate)
{
	double length = 0;
	for (int n = 0; n < 3; n++)
	{
		length += vector[n] * vector[n];
	}
	length = sqrt(length);

	for (int n = 0; n < 3; n++)
	{
		vector[n] = (vector[n] / length) / rate;
	}
}

/**
	Get Triangle's normal vectors
*/
void GetTriangleNormalVectors()
{
	triangleNormalVectors = new double*[nPolygons];

	for (int polygon = 0; polygon < nPolygons; polygon++)
	{	// read coordinates of points 
		triangleNormalVectors[polygon] = new double[3];

		for (int n = 0; n < 3; n++)
		{	// initialize triangleMiddlePoint
			triangleNormalVectors[polygon][n] = 0;
		}

		double v1[3], v2[3];	// v1 = p1 - p0, v2 = p2 - p0

		for (int n = 0; n < 3; n++)
		{
			v1[n] = points[polygons[polygon][1] - 1][n] - points[polygons[polygon][0] - 1][n];
			v2[n] = points[polygons[polygon][2] - 1][n] - points[polygons[polygon][0] - 1][n];
		}

		for (int n = 0; n < 3; n++)
		{	// cross product v1, v2
			triangleNormalVectors[polygon][n] = (v1[(n + 1) % 3] * v2[(n + 2) % 3]) - (v2[(n + 1) % 3] * v1[(n + 2) % 3]);
		}

		NormalizeVectors(triangleNormalVectors[polygon], 15.0);
	}
}

/*
	Draw middle points
*/
void DrawPoints()
{
	for (int polygon = 0; polygon < nPolygons; polygon++)
	{
		glColor3f(0, 0, 0);
		glPointSize(2);
		glBegin(GL_POINTS);
		{	// draw normal vector = normalVector - middlePoint
			glVertex3f(
				triangleMiddlePoints[polygon][0],
				triangleMiddlePoints[polygon][1],
				triangleMiddlePoints[polygon][2]
			);	// middle point
		}
		glEnd();
	}
}

/*
	Draw Normal vectors
*/
void DrawNormalVectors()
{
	for (int polygon = 0; polygon < nPolygons; polygon++)
	{
		glColor3f(0, 0, 0);
		glBegin(GL_LINES);
		{	// draw normal vector = normalVector - middlePoint
			glVertex3f(
				triangleMiddlePoints[polygon][0],
				triangleMiddlePoints[polygon][1],
				triangleMiddlePoints[polygon][2]
			);	// middle point
			glVertex3f(
				triangleNormalVectors[polygon][0] + triangleMiddlePoints[polygon][0],
				triangleNormalVectors[polygon][1] + triangleMiddlePoints[polygon][1],
				triangleNormalVectors[polygon][2] + triangleMiddlePoints[polygon][2]
			);
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
	DrawBunny();
	//DrawPoints();
	DrawNormalVectors();

	// Flush 
	glutSwapBuffers();
	glFlush();
}

void init(void)
{
	viewer[0] = 1.3;
	viewer[1] = 1.3;
	viewer[2] = 1.3;

	// read Bunny
	ReadBunny("bunny_origin.txt");

	GetTriangleMiddlepoints();
	GetTriangleNormalVectors();

	cout << "fin?" << endl;
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