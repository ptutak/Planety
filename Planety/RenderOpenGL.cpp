#include "RenderOpenGL.h"
#include <cstdio>
#include <string>
enum
{
	FULL_WINDOW, // aspekt obrazu - ca�e okno
	ASPECT_1_1, // aspekt obrazu 1:1
	WIRE_SPHERE, // kula
	WIRE_CONE, // sto�ek
	EXIT // wyj�cie
};

GLdouble eyex = 0;
GLdouble eyey = 0;
GLdouble eyez = 3;

GLdouble centerx = 0;
GLdouble centery = 0;
GLdouble centerz = 0;

int aspect = ASPECT_1_1;

GLdouble left = -10.0;
GLdouble right = 10.0;
GLdouble bottom = -10.0;

GLdouble top = 10.0;
GLdouble _near = 50.0;
GLdouble _far = 155.0;

GLdouble scale = 1.0;

GLdouble rotatex = 0.0;
GLdouble rotatey = 0.0;
GLdouble rotatez = 0.0;

GLdouble translatex = 0.0;
GLdouble translatey = 0.0;
GLdouble translatez = 0.0;

int button_state = GLUT_UP;

// po�o�enie kursora myszki

int button_x, button_y;

int argcp = 0;
char* argv = nullptr;

void origin(void) {
	glLineWidth(3.0);
	glBegin(GL_LINES);
	glColor3f(1.0, 0.0, 0.0);

	glVertex3d(0.0, 0.0, 0.0);
	glVertex3d(1.0, 0.0, 0.0);
	glVertex3d(1.0, 0.0, 0.0);
	glVertex3d(0.7, 0.3, 0.0);
	
	glColor3f(0.0, 0.0, 1.0);

	glVertex3d(0.0, 0.0, 0.0);
	glVertex3d(0.0, 1.0, 0.0);
	glVertex3d(0.0, 1.0, 0.0);
	glVertex3d(0.0, 0.7, 0.3);

	glColor3f(0.0, 1.0, 0.0);

	glVertex3d(0.0, 0.0, 1.0);
	glVertex3d(0.0, 0.0, 0.0);
	glVertex3d(0.0, 0.0, 1.0);
	glVertex3d(0.3, 0.0, 0.7);

	glEnd();

	glColor3f(1.0, 0.0, 0.0);
	glRasterPos3d(1.0, -0.25, -0.25);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, static_cast<int>('x'));
	glColor3f(0.0, 0.0, 1.0);
	glRasterPos3d(-0.25, 1.0, -0.25);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, static_cast<int>('y'));
	glColor3f(0.0, 1.0, 0.0);
	glRasterPos3d(-0.25, -0.25, 1.0);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, static_cast<int>('z'));

	glLineWidth(1.0);
}

void drawString(void * font, char *s, int x, int y, double z) {
	GLdouble xx = (right - left) / glutGet(GLUT_WINDOW_WIDTH)*static_cast<GLdouble>(x) + left;
	GLdouble yy = (top - bottom) / glutGet(GLUT_WINDOW_HEIGHT)*static_cast<GLdouble>(y) + top;
	
	if (aspect == ASPECT_1_1) {
		if ((glutGet(GLUT_WINDOW_WIDTH) < glutGet(GLUT_WINDOW_HEIGHT)) && glutGet(GLUT_WINDOW_WIDTH) > 0)
			yy = yy*glutGet(GLUT_WINDOW_HEIGHT) / glutGet(GLUT_WINDOW_WIDTH);
		else if ((glutGet(GLUT_WINDOW_WIDTH) >= glutGet(GLUT_WINDOW_HEIGHT)) && glutGet(GLUT_WINDOW_HEIGHT) > 0)
			xx = xx*glutGet(GLUT_WINDOW_WIDTH) / glutGet(GLUT_WINDOW_HEIGHT);
	}

	glRasterPos3d(xx, yy, z);
	for (unsigned int i = 0; i < strlen(s); i++)
		glutBitmapCharacter(font, s[i]);
}

void display(void) {
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslated(0, 0, -(_near + _far) / 2);

	glColor3d(0.0, 0.0, 0.0);
	drawString(GLUT_BITMAP_HELVETICA_12, "test",0 , 0,_near);

	glPushMatrix();
	glRotated(rotatex, 1.0, 0.0, 0.0);
	glRotated(rotatey, 0.0, 1.0, 0.0);
	glRotated(rotatez, 0.0, 0.0, 1.0);
	glScaled(3.0, 3.0, 3.0);
	origin();
	glPopMatrix();
	
	glRotated(rotatex, 1.0, 0.0, 0.0);
	glRotated(rotatey, 0.0, 1.0, 0.0);
	glRotated(rotatez, 0.0, 0.0, 1.0);

	glTranslated(translatex, translatey, translatez);

	glScaled(scale, scale, scale);
	
	glColor3f(0.0, 0.0, 0.0);
	


//	glutSolidCylinder(1.0, 2.0, 30, 5);
	
	

	glFlush();
	glutSwapBuffers();
}

void reshape(int width, int height) {
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	if (aspect == ASPECT_1_1) {
		if (width < height && width > 0)
			glFrustum(left, right, bottom * height / width,top * height / width, _near, _far);
		else if (width >= height && height > 0)
				glFrustum(left * width / height, right * width / height, bottom, top, _near, _far);
	}
	else
		glFrustum(left, right, bottom, top, _near, _far);
	display();
}

void keyboard(unsigned char key, int x, int y) {
	if (key == '+')
		rotatez +=1.5;
	if (key == '-')
		rotatez -= 1.5;
	if (key == 27)
		exit(0);
	reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
}

void specialKeys(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_LEFT:
		translatex -= 0.7;
		break;
	case GLUT_KEY_RIGHT:
		translatex += 0.7;
		break;
	case GLUT_KEY_UP:
		translatey += 0.7;
		break;
	case GLUT_KEY_DOWN:
		translatey -= 0.7;
		break;
	case GLUT_KEY_PAGE_UP:
		translatez += 0.7;
		break;
	case GLUT_KEY_PAGE_DOWN:
		translatez -= 0.7;
		break;
	}
	reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
}

void mouseButton(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON)
	{
		button_state = state;
		if (state == GLUT_DOWN)
		{
			button_x = x;
			button_y = y;
		}
	}
	else if ((button == 3) || (button == 4)) {
		if (state == GLUT_UP) 
			return;
		if (button == 3)
			scale += 0.05;
		if (button == 4)
			scale -= 0.05;
		if (scale < 0)
			scale = 0.0;
		reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	}
}

void mouseMotion(int x, int y) {
	if (button_state == GLUT_DOWN)
	{
		rotatex += 5.0 * (bottom - top) / glutGet(GLUT_WINDOW_HEIGHT) *(y - button_y);
		button_y = y;

		rotatey += 5.0 * (right - left) / glutGet(GLUT_WINDOW_WIDTH) *(x - button_x);
		button_x = x;
		reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	}
}
void menu(int value)
{
	switch (value)
	{
		// obszar renderingu - ca�e okno
	case FULL_WINDOW:
		aspect = FULL_WINDOW;
		reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		break;

		// obszar renderingu - aspekt 1:1
	case ASPECT_1_1:
		aspect = ASPECT_1_1;
		reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		break;
		// wyj�cie
	case EXIT:
		exit(0);
	}
}

void startRendering(gravityField* gravField) {
	glutInit(&argcp, &argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(glutGet(GLUT_SCREEN_WIDTH),static_cast<int>(glutGet(GLUT_SCREEN_HEIGHT)*0.94));
	glutCreateWindow("test 1");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKeys);
	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMotion);
	int menuAspect=glutCreateMenu(menu);
	glutAddMenuEntry("aspekt 1:1", ASPECT_1_1);
	glutAddMenuEntry("cale okno", FULL_WINDOW);
	int menuObject = glutCreateMenu(menu);
	// menu g��wne
	glutCreateMenu(menu);
	glutAddSubMenu("Aspekt obrazu", menuAspect);
	glutAddSubMenu("Obiekt", menuObject);
	glutAddMenuEntry("Wyjscie", EXIT);
	
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutMainLoop();
}
