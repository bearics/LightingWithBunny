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
#define TIMER 50
#define ROTATE_START 2
#define ROTATE_SPEED 2

static bool option[3];
static float viewer[3];
static int nPoints;
static int nPolygons;
static GLfloat** points;
static int** polygons;
static GLfloat** triangleMiddlePoints;
static GLfloat** triangleNormalVectors;

static GLfloat** rotateMatrix;

static int rotateTime;	// set roate 
static GLfloat** rotatePoints;

static int pointLightRotateSpeed;
static GLUquadric* pointLightSphere;

static int directLightSpeed;
static GLUquadric* directLightCylinder;
static int directLightTime;	// set roate 
static GLfloat directLight[4];



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

		points = new GLfloat*[nPoints];

		for (int n = 0; n < nPoints; n++)
		{	// read coordinates of points 
			points[n] = new GLfloat[3];

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

/*
	set timer spped
*/
void MyTimer(int value)
{
	rotateTime = (rotateTime + pointLightRotateSpeed) % 360;
	//directLightTime = (directLightTime + directLightSpeed) % 360;
	glutPostRedisplay();
	glutTimerFunc(TIMER, MyTimer, 1);
}

/**
	Draw Bunny with triangles
*/
void DrawBunny()
{
	glColor3f(1, 0.8, 0.8);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	for (int polygon = 0; polygon < nPolygons; polygon++)
	{	// Draw Bunny as Triangles with line frame
		glPointSize(20);
		glBegin(GL_POINTS);
		{
			glVertex3fv(rotatePoints[rotateTime / ROTATE_SPEED]);
		}
		glEnd();
		glBegin(GL_TRIANGLES);
		{
			for (int point = 0; point < 3; point++)
			{
				glNormal3fv(triangleNormalVectors[polygon]);
				glVertex3fv(points[polygons[polygon][point] - 1]);
			}
		}
		glEnd();
	}
	glDisable(GL_LIGHTING);
	
	glColor3f(0, 0, 0);
	for (int polygon = 0; polygon < nPolygons; polygon++)
	{	// draw line
		glBegin(GL_LINES);
		{
			for (int i = 0; i < 3; i++)
			{
				glVertex3fv(points[polygons[polygon][i] - 1]);
				glVertex3fv(points[polygons[polygon][(i + 1) % 3] - 1]);
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
	triangleMiddlePoints = new GLfloat*[nPolygons];

	for (int polygon = 0; polygon < nPolygons; polygon++)
	{	// read coordinates of points 
		triangleMiddlePoints[polygon] = new GLfloat[3];

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
void NormalizeVectors(GLfloat* vector, double rate)
{
	GLfloat length = 0;
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
	triangleNormalVectors = new GLfloat*[nPolygons];

	for (int polygon = 0; polygon < nPolygons; polygon++)
	{	// read coordinates of points 
		triangleNormalVectors[polygon] = new GLfloat[3];

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
			glVertex3fv(triangleMiddlePoints[polygon]);	// middle point
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
			glVertex3fv(triangleMiddlePoints[polygon]);	// middle point
			glVertex3f(
				triangleNormalVectors[polygon][0] + triangleMiddlePoints[polygon][0],
				triangleNormalVectors[polygon][1] + triangleMiddlePoints[polygon][1],
				triangleNormalVectors[polygon][2] + triangleMiddlePoints[polygon][2]
			);
		}
		glEnd();
	}
}

void InitRotateAboutAxis(double a, GLfloat x, GLfloat y, GLfloat z)
{	// Make the matrix R that rotates by a(=alpha) about a axis v

	//double v[3];
	//for (int i = 0; i < 3; i++)
	//	v[i] = _v[3];

	GLfloat vec[3] = { x, y, z };
	NormalizeVectors(vec, 1);

	rotateMatrix = new GLfloat*[3];
	for (int i = 0; i < 3; i++)
		rotateMatrix[i] = new GLfloat[3];

	x = vec[0];
	y = vec[1];
	z = vec[2];
	double cosa = cos(a);
	double sina = sin(a);

	// axis-angle rotation
	rotateMatrix[0][0] = (x*x) + (1 - (x*x))* cosa + sina * 0;
	rotateMatrix[0][1] = (x*y) + (0 - (x*y))* cosa + sina * (-z);
	rotateMatrix[0][2] = (x*z) + (0 - (x*z))* cosa + sina * y;
	rotateMatrix[1][0] = (y*x) + (0 - (y*x))* cosa + sina * z;
	rotateMatrix[1][1] = (y*y) + (1 - (y*y))* cosa + sina * 0;
	rotateMatrix[1][2] = (y*z) + (0 - (y*z))* cosa + sina * (-x);
	rotateMatrix[2][0] = (z*x) + (0 - (z*x))* cosa + sina * (-y);
	rotateMatrix[2][1] = (z*y) + (0 - (z*y))* cosa + sina * x;
	rotateMatrix[2][2] = (z*z) + (1 - (z*z))* cosa + sina * 0;

}

void MatMatrix4x1(GLfloat** rotateMatrix, GLfloat* point, GLfloat* result)
{
	for (int row = 0; row < 3; row++)
	{
		result[row] = 0;
		for (int col = 0; col < 3; col++)
		{
			result[row] += rotateMatrix[row][col] * point[col];
		}
	}

}

/**
	Init Rotate Points(pre-make rotated points)
*/
void InitRotatePoints()
{
	rotatePoints = new GLfloat*[360 / ROTATE_SPEED];

	rotatePoints[0] = new GLfloat[4];
	rotatePoints[0][0] = 0;
	rotatePoints[0][1] = 0;
	rotatePoints[0][2] = ROTATE_START;
	rotatePoints[0][3] = 1;

	for (int n = 1; n < 360 / ROTATE_SPEED; n++)
	{
		rotatePoints[n] = new GLfloat[4];
		MatMatrix4x1(rotateMatrix, rotatePoints[n-1], rotatePoints[n]);
		rotatePoints[n][3] = 1;
	}

	pointLightSphere = gluNewQuadric();
}

/**
	Draw Point light sphere
*/
void DrawPointLightShpere()
{
	glDisable(GL_LIGHTING);
	glPushMatrix();
	{	// draw point light
		glColor3f(0, 1, 1);
		glRotatef((GLfloat)rotateTime, 1.0, 1.0, 1.0);
		glTranslatef(0, 0, ROTATE_START);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		gluSphere(pointLightSphere, 0.1, 20, 20);
	}
	glPopMatrix();
	glEnable(GL_LIGHTING);
}
/**
	Set Point light position
*/
void SetPointLight()
{
	float ambientColor[] = { 0.2f, 0.2f, 0.2f, 1.0f };         
	float diffuseColor[] = { 8.0f, 8.0f, 8.0f, 1.0f };         
	//float specularColor[] = { 1.0f,  1.0f,  1.0f, 0.0f };        //¹æ»ç±¤

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientColor);       
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseColor);       
	glLightfv(GL_LIGHT0, GL_POSITION, rotatePoints[rotateTime / ROTATE_SPEED]);
}



void RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the window

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(viewer[0], viewer[1], viewer[2], 0, 0, 0, 0, 1, 0);

	// Draw cooddination system
	DrawCoordinationSystem(20);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	//glEnable(GL_LIGHT2);

	if (option[0])
	{
		pointLightRotateSpeed = ROTATE_SPEED;
		DrawPointLightShpere();
		SetPointLight();
	}
	else
	{
		pointLightRotateSpeed = 0;
		glDisable(GL_LIGHT0);
	}



	// Draw Bunny
	DrawBunny();
	DrawNormalVectors();


	// Flush 
	glutSwapBuffers();
	glFlush();
}

void init(void)
{
	// set viewer
	viewer[0] = 3.2;
	viewer[1] = 3.2;
	viewer[2] = 3.2;

	// init option
	for (int i = 0; i < 3; i++)
		option[i] = FALSE;

	// read Bunny
	ReadBunny("bunny_origin.txt");

	GetTriangleMiddlepoints();
	GetTriangleNormalVectors();

	InitRotateAboutAxis((ROTATE_SPEED * PI) / 180.0 , 1, 1, 1);

	// point light init
	InitRotatePoints();


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
	if (key == 'p') option[0] = option[0] ? FALSE : TRUE;
	if (key == 'd') option[1] = option[1] ? FALSE : TRUE;
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
	glutTimerFunc(TIMER, MyTimer, 1);
	glutMainLoop();
}