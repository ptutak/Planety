#include "RenderOpenGL.h"
#include <cstdio>
#include <string>
#include <thread>
enum
{
	AUTOMATIC_CAMERA, //nie zaimplementowano
	MANUAL_CAMERA, 
	EXIT // wyjœcie
};

int aspect = MANUAL_CAMERA;
int argcp = 0;
char* argv = nullptr;

gravityField** field = nullptr;

GLdouble left = -1.0;
GLdouble right = 1.0;
GLdouble bottom = -1.0;
GLdouble top = 1.0;
GLdouble _near = 1.0;
GLdouble _far = 5.0;

GLdouble scale = 1.0;

GLdouble rotatex = 0.0;
GLdouble rotatey = 0.0;
GLdouble rotatez = 0.0;

GLdouble translatex = 0.0;
GLdouble translatey = 0.0;
GLdouble translatez = 0.0;

int buttonState = GLUT_UP;
int mouseButtonX;
int mouseButtonY;


void drawCircle(double radius)
{
	const double DEG2RAD = 3.14159 / 180;
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 360; i++) {
		double degInRad = i*DEG2RAD;
		glVertex3d(cos(degInRad)*radius, sin(degInRad)*radius, 0.0);
	}
	glEnd();
}

void drawOrigin(void) {
	glLineWidth(3.0);
	glBegin(GL_LINES);
	glColor3d(1.0, 0.0, 0.0);

	glVertex3d(0.0, 0.0, 0.0);
	glVertex3d(1.0, 0.0, 0.0);
	glVertex3d(1.0, 0.0, 0.0);
	glVertex3d(0.7, 0.15, 0.0);

	glColor3d(0.0, 0.0, 1.0);

	glVertex3d(0.0, 0.0, 0.0);
	glVertex3d(0.0, 1.0, 0.0);
	glVertex3d(0.0, 1.0, 0.0);
	glVertex3d(0.0, 0.7, 0.15);

	glColor3d(0.0, 1.0, 0.0);

	glVertex3d(0.0, 0.0, 1.0);
	glVertex3d(0.0, 0.0, 0.0);
	glVertex3d(0.0, 0.0, 1.0);
	glVertex3d(0.15, 0.0, 0.7);

	glEnd();

	glColor3d(1.0, 0.0, 0.0);
	glRasterPos3d(1.0, -0.25, -0.25);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, static_cast<int>('x'));

	glColor3d(0.0, 0.0, 1.0);
	glRasterPos3d(-0.25, 1.0, -0.25);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, static_cast<int>('y'));

	glColor3d(0.0, 1.0, 0.0);
	glRasterPos3d(-0.25, -0.25, 1.0);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, static_cast<int>('z'));
	glLineWidth(1.0);
}

void drawString(std::string s, int x, int y, void * font) {
	GLdouble xx;
	GLdouble yy;
	if (aspect == MANUAL_CAMERA) {
		xx = (right - left) / glutGet(GLUT_WINDOW_WIDTH)*static_cast<GLdouble>(x) + left;
		yy = (top - bottom) / glutGet(GLUT_WINDOW_HEIGHT)*static_cast<GLdouble>(y) + top;
		if ((glutGet(GLUT_WINDOW_WIDTH) < glutGet(GLUT_WINDOW_HEIGHT)) && glutGet(GLUT_WINDOW_WIDTH) > 0)
			yy = yy*glutGet(GLUT_WINDOW_HEIGHT) / glutGet(GLUT_WINDOW_WIDTH);
		else if ((glutGet(GLUT_WINDOW_WIDTH) >= glutGet(GLUT_WINDOW_HEIGHT)) && glutGet(GLUT_WINDOW_HEIGHT) > 0)
			xx = xx*glutGet(GLUT_WINDOW_WIDTH) / glutGet(GLUT_WINDOW_HEIGHT);
	}
	glPushMatrix();
	glLoadIdentity();
	glRasterPos3d(xx, yy, -_near - 0.05);
	for (unsigned int i = 0; i < s.length(); i++)
		glutBitmapCharacter(font, s[i]);
	glPopMatrix();
}
void drawStream(std::istream& str, int x, int y, void* font, int fontSize) {
	int line = 0;
	while (!str.eof()) {
		std::string tmp;
		std::getline(str, tmp);
		drawString(tmp, 5 + x, -11 + y - fontSize*line, font);
		line++;
	}
}

void drawAxis(void) {
	glPushMatrix();
	glScaled((_far - _near)*1.3, (_far - _near)*1.3,( _far - _near)*1.3);
	
	glBegin(GL_LINES);
	glColor3d(1, 0, 0);
	glVertex3d(-1, 0, 0);
	glVertex3d(1, 0, 0);
	glColor3d(0, 0, 1);
	glVertex3d(0, -1, 0);
	glVertex3d(0, 1, 0);
	glColor3d(0, 1, 0);
	glVertex3d(0, 0, -1);
	glVertex3d(0, 0, 1);
	glEnd();
	
	glPopMatrix();
}
void initDisplayMatrixModeBackground(void) {
	glClearColor(1., 1., .94, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslated(0, 0, -_near*3);

	glRotated(rotatex, 1.0, 0.0, 0.0);
	glRotated(rotatey, 0.0, 1.0, 0.0);
	glRotated(rotatez, 0.0, 0.0, 1.0);

	glTranslated(translatex, translatey, translatez);

	drawAxis();

	glPushMatrix();
	glScaled(3*_near/10, 3*_near / 10, 3*_near / 10);
	drawOrigin();
	glPopMatrix();

	glScaled(scale, scale, scale);
}

void drawObjectsList(void) {
	int leftPosition = 0;
	int topPosition = 0;
	for (const auto i : (*field)->getObjects()) {
		std::lock_guard<std::mutex> lg(getReadWriteMutex());
		std::stringstream strm;
		strm << *i;
		drawStream(strm, leftPosition * 80, topPosition*-150, GLUT_BITMAP_HELVETICA_12, 12);
		++leftPosition;
		if (((leftPosition + 1) * 80) > glutGet(GLUT_WINDOW_WIDTH)){
			leftPosition = 0;
			topPosition++;
		}
	}
}

void drawObjects(void) {
	glColor3d(0, 0, 0);
	for (const auto i : (*field)->getObjects()) {
		glPushMatrix();
		{
			std::lock_guard<std::mutex> lg(getReadWriteMutex());
			glTranslated(i->getX(), i->getY(), i->getZ());
			glRotated(90, 1, 0, 0);
			glutWireSphere(i->getDiameter() / 2, 40, 20);
		}
		glPopMatrix();
	}
}

void display(void) {
	if (GetAsyncKeyState(VK_ESCAPE))
		glutLeaveMainLoop();

	initDisplayMatrixModeBackground();
	glColor3d(0.0, 0.0, 0.0);
	drawObjectsList();
	drawObjects();
	
	glFlush();
	glutSwapBuffers();
}

void calculateScene(void) {
	double maxX;
	double maxY;
	double maxZ;
	double minX;
	double minY;
	double minZ;
	double maxD;
	{
		std::lock_guard<std::mutex> lg(getReadWriteMutex());
		maxX = (*field)->getMaxX();
		maxY = (*field)->getMaxY();
		maxZ = (*field)->getMaxZ();
		minX = (*field)->getMinX();
		minY = (*field)->getMinY();
		minZ = (*field)->getMinZ();
		maxD = (*field)->getMaxD();
	}
	double maxSize = abs(maxX);
	if (abs(maxY) > maxSize)
		maxSize = abs(maxY);
	if (abs(maxZ) > maxSize)
		maxSize = abs(maxZ);
	if (abs(minX) > maxSize)
		maxSize = abs(minX);
	if (abs(minY) > maxSize)
		maxSize = abs(minY);
	if (abs(minZ) > maxSize)
		maxSize = abs(minZ);
	top = maxSize+maxD;
	bottom = -maxSize-maxD;
	right = maxSize+maxD;
	left = -maxSize-maxD;
	_near = (maxSize+maxD)*2;
	_far = 1000 * (maxSize + maxD) + _near;
}

void reshape(int width, int height) {
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	calculateScene();
	if (aspect == MANUAL_CAMERA) {
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
	{
		std::lock_guard<std::mutex> lg(getMultiplierMutex());
		(*field)->addMultiplier(0.5);
	}
	if (key == '-')
	{
		std::lock_guard<std::mutex> lg(getMultiplierMutex());
		(*field)->addMultiplier(-0.5);
	}

	reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
}

void specialKeys(int key, int x, int y)
{
	double scaleTr = (right - left)*0.05;
	switch (key) {
	case GLUT_KEY_LEFT:
		translatex -= scaleTr;
		break;
	case GLUT_KEY_RIGHT:
		translatex += scaleTr;
		break;
	case GLUT_KEY_UP:
		translatey += scaleTr;
		break;
	case GLUT_KEY_DOWN:
		translatey -= scaleTr;
		break;
	case GLUT_KEY_PAGE_UP:
		translatez += scaleTr;
		break;
	case GLUT_KEY_PAGE_DOWN:
		translatez -= scaleTr;
		break;
	}
	reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
}

void mouseButton(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON)
	{
		buttonState = state;
		if (state == GLUT_DOWN)
		{
			mouseButtonX = x;
			mouseButtonY = y;
		}
	}
	else if ((button == 3) || (button == 4)) {
		if (state == GLUT_UP) 
			return;
		if (button == 3)
			scale *= 1.05;
		if (button == 4)
			scale *= 0.95;
		reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	}
}

void mouseMotion(int x, int y) {
	if (buttonState == GLUT_DOWN)
	{
		rotatex += .1 * (y - mouseButtonY);
		mouseButtonY = y;

		rotatey += .1 * (x - mouseButtonX);
		mouseButtonX = x;
		reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	}
}
void menu(int value)
{
	switch (value)
	{
	case AUTOMATIC_CAMERA:
		aspect = AUTOMATIC_CAMERA;
		reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		break;
	case MANUAL_CAMERA:
		aspect = MANUAL_CAMERA;
		reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		break;
	case EXIT:
		glutLeaveMainLoop();
	}
}

void initFunc(void) {
	glutInit(&argcp, &argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(glutGet(GLUT_SCREEN_WIDTH), static_cast<int>(glutGet(GLUT_SCREEN_HEIGHT)*0.94));
	glutCreateWindow("Planety");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKeys);
	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMotion);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
}

void initMenu(void) {
	int menuAspect = glutCreateMenu(menu);
	glutAddMenuEntry("kamera manualna", MANUAL_CAMERA);
//	glutAddMenuEntry("kamera automatyczna", AUTOMATIC_CAMERA);

	int menuObject = glutCreateMenu(menu);
	// menu g³ówne

	glutCreateMenu(menu);
	glutAddSubMenu("Aspekt obrazu", menuAspect);
	glutAddSubMenu("Obiekt", menuObject);
	glutAddMenuEntry("Wyjscie", EXIT);

	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void setGravField(gravityField** gravField) {
	if (gravField)
		field = gravField;
	else
		throw threadExit("gravField = nullptr",-1);
	if (!*gravField)
		throw threadExit("*gravField = nullptr",-1);
}

void startRendering(gravityField** gravField) {
	setGravField(gravField);
	initFunc();
	initMenu();
	glutMainLoop();
}
