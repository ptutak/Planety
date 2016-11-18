#include "RenderOpenGL.h"

enum
{
	AUTOMATIC_CAMERA, //nie zaimplementowano
	MANUAL_CAMERA, 
	EXIT
};

int aspect = MANUAL_CAMERA;
int argcp = 0;
char* argv = nullptr;

gravityField** field = nullptr;
std::mutex* fieldMutex = nullptr;

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

	glColor3d(0.0, 0.9, 0.10);

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

	glColor3d(0.0, 0.9, 0.10);
	glRasterPos3d(-0.25, -0.25, 1.0);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, static_cast<int>('z'));
	glLineWidth(1.0);
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
	glColor3d(0, 0.9, 0.1);
	glVertex3d(0, 0, -1);
	glVertex3d(0, 0, 1);
	glEnd();
	
	glPopMatrix();
}

void drawObjects(void) {
	std::lock_guard<std::mutex> lg(*fieldMutex);
	glColor3d(0, 0, 0);
	for (const auto i : (*field)->getObjects()) {
		double x, y, z, d;
		std::string name;
		{
			std::lock_guard<std::mutex> lg2((*field)->objectsMutex);
			x = i->getX();
			y = i->getY();
			z = i->getZ();
			d = i->getDiameter();
			name = i->getName();
		}
		d = d / 2.0;
		glPushMatrix();
		glTranslated(x, y, z);
		glRasterPos3d(0.95*d, 0.95*d, 0.0);
		for (unsigned int i = 0; i < name.length(); i++)
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, name[i]);
		glBegin(GL_LINES);
		glVertex3d(0.707*d, 0.707*d, 0.0);
		glVertex3d(0.95*d, 0.95*d, 0.0);
		glEnd();
		glRotated(90, 1, 0, 0);
		glutWireSphere(d, 40, 20);
		glPopMatrix();
	}
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
	glRasterPos3d(xx, yy, -_near - 0.0001*_near);
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
void drawObjectsList(void) {
	int leftPosition = 0;
	int topPosition = 0;
	std::lock_guard<std::mutex> lg(*fieldMutex);
	for (const auto i : (*field)->getObjects()) {
		std::stringstream strm;
		{
			std::lock_guard<std::mutex> lg((*field)->objectsMutex);
			strm << *i;
		}
		drawStream(strm, leftPosition * 110, topPosition*-150, GLUT_BITMAP_HELVETICA_12, 12);
		++leftPosition;
		if (((leftPosition + 1) * 110) > glutGet(GLUT_WINDOW_WIDTH)){
			leftPosition = 0;
			topPosition++;
		}
	}
}

std::string reformatSec(double sec) {
	double restSec;
	unsigned long long mins;
	unsigned long hours;
	unsigned long days;
	unsigned long years;
	mins = static_cast<unsigned long long>(sec) / 60;
	restSec = sec - static_cast<double>(mins * 60);
	hours = static_cast<unsigned long>(mins / 60);
	mins = mins % 60;
	days = hours / 24;
	hours = hours % 24;
	years = days / 365;
	days = days % 365;
	std::stringstream tmp;
	if (years)
		tmp << years<<"y ";
	if (days)
		tmp << days << "d ";
	if (hours)
		tmp << hours << "h ";
	if (mins)
		tmp << mins << "m ";
	tmp << restSec << "s";
	return tmp.str();
}

void drawParameters(void) {
	double multiplier;
	double simulTime;
	int lastFrame = getInfo().getLastFrame();
	int constFrame = getInfo().getConstFrame();
	double realTime = getInfo().getRealTime();
	{
		std::lock_guard<std::mutex> lg(*fieldMutex);
		multiplier=(*field)->getTimeMultiplier();
		simulTime = (*field)->getSimulTime();
	}

	std::stringstream tmp;
	tmp << constFrame;
	drawString("Stala ramka czasu:" + tmp.str() + "ms", 15, -glutGet(GLUT_WINDOW_HEIGHT) + 87, GLUT_BITMAP_HELVETICA_18);
	tmp.str("");
	tmp.clear();
	tmp << multiplier;
	drawString("Mnoznik czasu:"+tmp.str()+"x", 15, -glutGet(GLUT_WINDOW_HEIGHT)+69, GLUT_BITMAP_HELVETICA_18);
	tmp.str("");
	tmp.clear();
	tmp << lastFrame;
	drawString("Czas opoznienia:" + tmp.str() + "ms", 15, -glutGet(GLUT_WINDOW_HEIGHT) + 51, GLUT_BITMAP_HELVETICA_18);
	drawString("Czas symulacji:" + reformatSec(simulTime), 15, -glutGet(GLUT_WINDOW_HEIGHT) + 33, GLUT_BITMAP_HELVETICA_18);
	drawString("Czas rzeczywisty:" + reformatSec(realTime), 15, -glutGet(GLUT_WINDOW_HEIGHT) + 15, GLUT_BITMAP_HELVETICA_18);
}

void initDisplayMatrixModeBackground(void) {
	glClearColor(1.0, 1.0, 0.94, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslated(0, 0, -_near * 3);

	glRotated(rotatex, 1.0, 0.0, 0.0);
	glRotated(rotatey, 0.0, 1.0, 0.0);
	glRotated(rotatez, 0.0, 0.0, 1.0);

	glTranslated(translatex, translatey, translatez);

	drawAxis();

	glPushMatrix();
	glScaled(3 * _near / 10, 3 * _near / 10, 3 * _near / 10);
	drawOrigin();
	glPopMatrix();

	glScaled(scale, scale, scale);
}

void display(void) {
	initDisplayMatrixModeBackground();
	glColor3d(0.0, 0.0, 0.0);
	drawObjectsList();
	drawParameters();
	drawObjects();
	glFlush();
	glutSwapBuffers();
}

void idle(void) {
	glutPostRedisplay();
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
		std::lock_guard<std::mutex> lg(*fieldMutex);
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
	switch (key) {
	case '+':
	{
		std::lock_guard<std::mutex> lg(*fieldMutex);
		(*field)->addMultiplier(0.5);
	}
	break;
	case '-':
	{
		std::lock_guard<std::mutex> lg(*fieldMutex);
		(*field)->addMultiplier(-0.5);
	}
	break;
	case '*':
	{
		std::lock_guard<std::mutex> lg(*fieldMutex);
		(*field)->multiplyMultiplier(2.0);
	}
	break;
	case '/':
	{
		std::lock_guard<std::mutex> lg(*fieldMutex);
		(*field)->multiplyMultiplier(0.5);
	}
	break;
	case ' ':
	{
		std::lock_guard<std::mutex> lg(*fieldMutex);
		(*field)->setTimeMultiplier(1.0);
	}
	break;
	case 8: 
		getInfo().resetClock();
		break;
	case 13:
		getInfo().startStopClock();
		break;
	case 27:
		glutLeaveMainLoop();
		break;
	}
	display();
//	reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
}

void specialKeys(int key, int x, int y)
{
	double scaleTr = (right - left)*0.03;
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
	case GLUT_KEY_SHIFT_L:
		{
			std::lock_guard<std::mutex> lg(*fieldMutex);
			(*field)->setTimeMultiplier(0.0);
		}
		break;
	case GLUT_KEY_SHIFT_R:
		{
			std::lock_guard<std::mutex> lg(*fieldMutex);
			(*field)->setTimeMultiplier(0.0);
		}
		break;
	}
	display();
//	reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
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
			scale /= .95;
		if (button == 4)
			scale *= 0.95;
		reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	}
}

void mouseMotion(int x, int y) {
	if (buttonState == GLUT_DOWN)
	{
		rotatex += 0.1 * (y - mouseButtonY);
		mouseButtonY = y;

		rotatey += 0.1 * (x - mouseButtonX);
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
	glutInitWindowSize(static_cast<int>(glutGet(GLUT_SCREEN_WIDTH)*0.9), static_cast<int>(glutGet(GLUT_SCREEN_HEIGHT)*0.9));
	glutCreateWindow("Planety");
	glutDisplayFunc(display);
	glutIdleFunc(idle);
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

	// menu g³ówne

	glutCreateMenu(menu);
	glutAddSubMenu("Tryb kamery", menuAspect);
	glutAddMenuEntry("Wyjscie", EXIT);

	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void setGravFieldMutex(gravityField** gravField,std::mutex* gravMutex) {
	if (gravField)
		field = gravField;
	else
		throw threadExit("gravField = nullptr",-1);
	if (!*gravField)
		throw threadExit("*gravField = nullptr",-1);
	if (gravMutex)
		fieldMutex = gravMutex;
	else
		throw threadExit("gravMutex = nullptr", -1);
}

void startRendering(gravityField** gravField, std::mutex* gravMutex) {
	setGravFieldMutex(gravField,gravMutex);
	initFunc();
	initMenu();
	glutMainLoop();
}
