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
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("My First OpenGl Program - Bhushan R. Kalamkar");

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
}

void unintialize(void)
{
	//*****CODE*********//
}

void reshape(int width, int height)
{
	//*******CODE**********//
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
}

void display(void)
{
	//******** CODE **********//
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glBegin(GL_TRIANGLES);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex2f(0.0f, 1.0f);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(-1.0f, -1.0f);

	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex2f(1.0f, -1.0f);
	glEnd();

	glFlush();
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
	case GLUT_LEFT_BUTTON :
		break;

	case GLUT_RIGHT_BUTTON :
		glutLeaveMainLoop();
	}
}