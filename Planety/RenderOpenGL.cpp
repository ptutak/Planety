#include "RenderOpenGL.h"
enum
{
	FULL_WINDOW, // aspekt obrazu - ca³e okno
	ASPECT_1_1, // aspekt obrazu 1:1
	WIRE_SPHERE, // kula
	WIRE_CONE, // sto¿ek
	WIRE_CUBE, // szeœcian
	WIRE_TORUS, // torus
	WIRE_DODECAHEDRON, // dwunastoœcian
	WIRE_TEAPOT, // czajnik
	WIRE_OCTAHEDRON, // oœmioœcian
	WIRE_TETRAHEDRON, // czworoœcian
	WIRE_ICOSAHEDRON, // dwudziestoœcian
	EXIT // wyjœcie
};

GLdouble eyex = 0;
GLdouble eyey = 0;
GLdouble eyez = 3;

GLdouble centerx = 0;
GLdouble centery = 0;
GLdouble centerz = 0;

int aspect = FULL_WINDOW;

int object = WIRE_SPHERE;

const GLdouble left = -10.0;
const GLdouble right = 10.0;
const GLdouble bottom = -10.0;

const GLdouble top = 10.0;
GLdouble _near = 50.0;
GLdouble _far = 155.0;

GLdouble scale = 1.0;

GLdouble rotatex = 0.0;
GLdouble rotatey = 0.0;

GLdouble translatex = 0.0;
GLdouble translatey = 0.0;

int button_state = GLUT_UP;

// po³o¿enie kursora myszki

int button_x, button_y;

int argcp = 0;
char* argv = nullptr;

void Cube3x3()
{
	glutWireCube(1.0);
	glTranslated(1.0, 0.0, 0.0);
	glutWireCube(1.0);
	glTranslated(0.0, -1.0, 0.0);
	glutWireCube(1.0);
	glTranslated(-1.0, 0.0, 0.0);
	glutWireCube(1.0);
	glTranslated(-1.0, 0.0, 0.0);
	glutWireCube(1.0);
	glTranslated(0.0, 1.0, 0.0);
	glutWireCube(1.0);
	glTranslated(0.0, 1.0, 0.0);
	glutWireCube(1.0);
	glTranslated(1.0, 0.0, 0.0);
	glutWireCube(1.0);
	glTranslated(1.0, 0.0, 0.0);
	glutWireCube(1.0);
	glTranslated(-1.0, -1.0, 0.0);
}


void Cube2x2()
{
	glutWireCube(1.0);
	glTranslated(1.0, 0.0, 0.0);
	glutWireCube(1.0);
	glTranslated(0.0, -1.0, 0.0);
	glutWireCube(1.0);
	glTranslated(-1.0, 0.0, 0.0);
	glutWireCube(1.0);
	glTranslated(0.5, 0.5, 0.0);
}

void display(void) {
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glColor3f(0.0, 0.0, 0.0);

	glTranslated(0, 0, -(_near + _far) / 2);

	glScaled(scale, scale, scale);


	glRotated(rotatex, 1.0, 0.0, 0.0);
	glRotated(rotatey, 0.0, 1.0, 0.0);
	
	Cube2x2();
	glColor3f(0.0, 1.0, 0.0);
	glutWireCube(1.0);

	glColor3f(1.0, 0.0, 0.0);

	glBegin(GL_LINES);

	glVertex3d(0.0, 0.0, 0.0);
	glVertex3d(1.0, 0.0, 0.0);
	
	glVertex3d(1.0, 0.0, 0.0);
	glVertex3d(0.7, 0.3, 0.0);

	glVertex3d(0.0, 0.0, 0.0);
	glVertex3d(0.0, 1.0, 0.0);

	glVertex3d(0.0, 0.0, 1.0);
	glVertex3d(0.0, 0.0, 0.0);


	glEnd();


	glTranslated(10.0, 0.0, 0.0);

	glColor3f(0.0, 1.0, 0.0);


	glTranslated(translatex, translatey, 0);

	glColor3f(0.0, 0.0, 1.0);

	glBegin(GL_LINES);

	glVertex3d(0.0, 0.0, 0.0);
	glVertex3d(1.0, 0.0, 0.0);

	glVertex3d(1.0, 0.0, 0.0);
	glVertex3d(0.7, 0.3, 0.0);

	glVertex3d(0.0, 0.0, 0.0);
	glVertex3d(0.0, 1.0, 0.0);

	glVertex3d(0.0, 0.0, 0.0);
	glVertex3d(0.0, 0.0, 1.0);

	glEnd();


	glFlush();
	glutSwapBuffers();
}

void reshape(int width, int height) {
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	if (aspect == ASPECT_1_1)
	{
		// wysokoœæ okna wiêksza od wysokoœci okna
		if (width < height && width > 0)
			glFrustum(left, right, bottom * height / width,top * height / width, _near, _far);
		else

			// szerokoœæ okna wiêksza lub równa wysokoœci okna
			if (width >= height && height > 0)
				glFrustum(left * width / height, right * width / height, bottom, top, _near, _far);

	}
	else
		glFrustum(left, right, bottom, top, _near, _far);
	display();
}

void keyboard(unsigned char key, int x, int y) {
	if (key == '+')
		scale +=0.1;
	if (key == '-')
		scale -= 0.1;
	if (scale < 0.0)
		scale = 0.0;
	if (key == 27)
		exit(0);

	reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
}

void specialKeys(int key, int x, int y)
{
	switch (key)
	{
		
	case GLUT_KEY_LEFT:
		rotatey -= 1;
		break;
	case GLUT_KEY_RIGHT:
		rotatey += 1;
		break;

	case GLUT_KEY_UP:
		rotatex -= 1;
		break;
	case GLUT_KEY_DOWN:
		rotatex += 1;
		break;
	}

	// odrysowanie okna
	reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
}

void mouseButton(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON)
	{
		// zapamiêtanie stanu lewego przycisku myszki
		button_state = state;

		// zapamiêtanie po³o¿enia kursora myszki
		if (state == GLUT_DOWN)
		{
			button_x = x;
			button_y = y;
		}
	}
}

void mouseMotion(int x, int y) {
	if (button_state == GLUT_DOWN)
	{
		translatex += 1.1 *(right - left) / glutGet(GLUT_WINDOW_WIDTH) *(x - button_x);
		button_x = x;
		translatey += 1.1 *(top - bottom) / glutGet(GLUT_WINDOW_HEIGHT) *(button_y - y);
		button_y = y;
		glutPostRedisplay();
	}
}
void menu(int value)
{
	switch (value)
	{
		// obszar renderingu - ca³e okno
	case FULL_WINDOW:
		aspect = FULL_WINDOW;
		reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		break;

		// obszar renderingu - aspekt 1:1
	case ASPECT_1_1:
		aspect = ASPECT_1_1;
		reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		break;

		// kula
	case WIRE_SPHERE:
		object = WIRE_SPHERE;
		display();
		break;

		// cylinder
	case WIRE_CONE:
		object = WIRE_CONE;
		display();
		break;

		// szeœcian
	case WIRE_CUBE:
		object = WIRE_CUBE;
		display();
		break;

		// torus
	case WIRE_TORUS:
		object = WIRE_TORUS;
		display();
		break;

		// dwunastoœcian
	case WIRE_DODECAHEDRON:
		object = WIRE_DODECAHEDRON;
		display();
		break;

		// czajnik
	case WIRE_TEAPOT:
		object = WIRE_TEAPOT;
		display();
		break;

		// oœmioœcian
	case WIRE_OCTAHEDRON:
		object = WIRE_OCTAHEDRON;
		display();
		break;

		// czworoœcian
	case WIRE_TETRAHEDRON:
		object = WIRE_TETRAHEDRON;
		display();
		break;

		// dwudziestoœcian
	case WIRE_ICOSAHEDRON:
		object = WIRE_ICOSAHEDRON;
		display();
		break;

		// wyjœcie
	case EXIT:
		exit(0);
	}
}

void startRendering(gravityField* gravField) {
	glutInit(&argcp, &argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(500,500);
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
	glutAddMenuEntry("Kula", WIRE_SPHERE);
	glutAddMenuEntry("Stozek", WIRE_CONE);
	glutAddMenuEntry("Szescian", WIRE_CUBE);
	glutAddMenuEntry("Torus", WIRE_TORUS);
	glutAddMenuEntry("Dwunastoscian", WIRE_DODECAHEDRON);
	glutAddMenuEntry("Czajnik", WIRE_TEAPOT);
	glutAddMenuEntry("Osmioscian", WIRE_OCTAHEDRON);
	glutAddMenuEntry("Czworoscian", WIRE_TETRAHEDRON);
	glutAddMenuEntry("Dwudziestoscian", WIRE_ICOSAHEDRON);
	// menu g³ówne
	glutCreateMenu(menu);
	glutAddSubMenu("Aspekt obrazu", menuAspect);
	glutAddSubMenu("Obiekt", menuObject);
	glutAddMenuEntry("Wyjscie", EXIT);
	
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutMainLoop();
}
