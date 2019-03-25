
/*###################################
## File        : My_First_OpenGl.c
## Author      : Bhushan R. Kalamkar
## Discription : First OpenGl program using GLUT.
##
######################################
*/

//************ Include Headers ****************//
#include<GL/freeglut.h>
#include<Windows.h>



//*************Global Variable ***************//
BOOL bIsFullScreen = FALSE;
float triangle_rotation_angle = 0.0f;
float cuboid_rotation_angle = 0.0f;

int main(int argc, char *argv[])
{
	//***** Function declaration *********//
	void intialize(void);
	void unintialize(void);
	void reshape(int, int);
	void display(void);
	
	void keyboard(unsigned char, int, int);
	void mouse(int, int, int, int);
	void update();

	//*****CODE *****************//
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE |GLUT_DEPTH | GLUT_RGBA);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Glut Pyramid and Cuboid - Bhushan R. Kalamkar");
	glutIdleFunc(update);
	intialize();

	//******Callback Functions *******//
	glutDisplayFunc(display);
	


	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutCloseFunc(unintialize);

	glutMainLoop();

	return (0);
}

void intialize(void)
{
	//*****CODE*********//
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);

	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

void unintialize(void)
{
	//*****CODE*********//
}

void reshape(int width, int height)
{
	//*******CODE**********//
	
	if (height == 0)
	{
		height = 1;
	}

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();


	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void update()
{
	if (triangle_rotation_angle > 360)
	{

		triangle_rotation_angle = 0.0f;
		cuboid_rotation_angle = 0.0f;
	}
	else {

		triangle_rotation_angle = triangle_rotation_angle + 2.0f;
		cuboid_rotation_angle = cuboid_rotation_angle + 2.0f;
	}

	glutPostRedisplay();
}
void display()
{
	//code

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	//Triangle

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-2.0f, 0.0f, -6.0f);
	glRotatef(triangle_rotation_angle, 0.0f, 1.0f, 0.0f);

	glBegin(GL_TRIANGLES);

	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);

	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);


	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);

	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);

	glEnd();


	//Cuboid

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(2.0f, 0.0f, -6.0f);
	glScaled(0.75, 0.75, 0.75);
	glRotatef(cuboid_rotation_angle, 1.0f, 1.0f, 1.0f);

	glBegin(GL_QUADS);
	//Top
	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(1.0, 1.0, -1.0);
	glVertex3f(-1.0, 1.0, -1.0);
	glVertex3f(-1.0, 1.0, 1.0);
	glVertex3f(1.0, 1.0, 1.0);
	//Bottom
	glColor3f(0.0, 1.0, 0.0);
	glVertex3f(1.0, -1.0, -1.0);
	glVertex3f(-1.0, -1.0, -1.0);
	glVertex3f(-1.0, -1.0, 1.0);
	glVertex3f(1.0, -1.0, 1.0);
	//Front
	glColor3f(0.0, 0.0, 1.0);
	glVertex3f(1.0, 1.0, 1.0);
	glVertex3f(-1.0, 1.0, 1.0);
	glVertex3f(-1.0, -1.0, 1.0);
	glVertex3f(1.0, -1.0, 1.0);
	//Back
	glColor3f(0.0, 1.0, 1.0);
	glVertex3f(1.0, 1.0, -1.0);
	glVertex3f(-1.0, 1.0, -1.0);
	glVertex3f(-1.0, -1.0, -1.0);
	glVertex3f(1.0, -1.0, -1.0);
	//Right
	glColor3f(1.0, 0.0, 1.0);
	glVertex3f(1.0, 1.0, -1.0);
	glVertex3f(1.0, 1.0, 1.0);
	glVertex3f(1.0, -1.0, 1.0);
	glVertex3f(1.0, -1.0, -1.0);
	//Left
	glColor3f(1.0, 1.0, 0.0);
	glVertex3f(-1.0, 1.0, -1.0);
	glVertex3f(-1.0, -1.0, -1.0);
	glVertex3f(-1.0, -1.0, 1.0);
	glVertex3f(-1.0, 1.0, 1.0);
	glEnd();



	glutSwapBuffers();
}



void keyboard(unsigned char key, int x, int y)
{
	//*********Code***************//
	switch (key)
	{
	case 27:
		glutLeaveMainLoop();
		break;

	case 'F':
	case 'f':
		if (FALSE == bIsFullScreen)
		{
			glutFullScreen();
			bIsFullScreen = TRUE;
		}
		else
		{
			glutLeaveFullScreen();
			bIsFullScreen = FALSE;
		}
		break;
	}
}

void mouse(int button, int state, int x, int y)
{
	//*******Code**********//
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		break;

	case GLUT_RIGHT_BUTTON:
		glutLeaveMainLoop();
	}
}