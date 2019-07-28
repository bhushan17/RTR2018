
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
int year = 0;
int day = 0;


int main(int argc, char *argv[])
{
	//***** Function declaration *********//
	void intialize(void);
	void unintialize(void);
	void reshape(int, int);
	void display(void);
	void keyboard(unsigned char, int, int);
	void mouse(int, int, int, int);

	//*****CODE *****************//
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE| GLUT_DEPTH | GLUT_RGBA);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("My First OpenGl Program - Bhushan Kalamkar");

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
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glShadeModel(GL_FLAT);
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
	gluPerspective(60.0f, GLfloat(width) / GLfloat(height), 1.0, 20.0);
}

void display(void)
{
	//******** CODE **********//
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	glPushMatrix();
	glutWireSphere(1.0f, 20.0f, 16.0f); // Draw Sun
	glRotatef(GLfloat(year), 0.0f, 1.0f, 0.0f);
	glTranslatef(2.0f, 0.0f, 0.0f);
	glRotatef(GLfloat(day), 0.0f, 1.0f, 0.0f);
	glutWireSphere(0.2f, 10.0f, 8.0f);//Draw Earth
	glPopMatrix();
	

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
	case 'Y':
		year = (year + 5) % 360;
		glutPostRedisplay();
		break;
	case 'y':
		year = (year - 5) % 360;
		glutPostRedisplay();
		break;
	case 'D':
		day = (day + 10) % 360;
		glutPostRedisplay();
		break;
	case 'd':
		day = (day - 10) % 360;
		glutPostRedisplay();
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