/*
Copyright 2016 Piotr Tutak

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http ://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#include "RenderOpenGL.h"

enum
{
	MANUAL_CAMERA,
	CENTER_VIEW,
	DESCRIPTION_ON_OFF,
	FLOATING_DESCRIPTION_ON_OFF,
	PARAMETERS_ON_OFF,
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
GLdouble _near = 3.0;
GLdouble _far = 5.0;

GLdouble scale = 1.0;
GLdouble maxDistance = 0.0;

GLdouble rotatex = 0.0;
GLdouble rotatey = 0.0;
GLdouble rotatez = 0.0;

GLdouble translatex = 0.0;
GLdouble translatey = 0.0;
GLdouble translatez = 0.0;

int buttonState = GLUT_UP;
int mouseButtonX;
int mouseButtonY;

bool toggleDescription = true;
bool toggleFloatingDescription = true;
bool toggleParameters = true;

int windowWidth = 0;
int windowHeight = 0;

constexpr double DEG2RAD = 180.0 / 3.14159;

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

void translateDrawAxisOrigin(void) {
	glTranslated(0, 0, -maxDistance * 3);
	
	glRotated(rotatex, 1.0, 0.0, 0.0);
	glRotated(rotatey, 0.0, 1.0, 0.0);
	glRotated(rotatez, 0.0, 0.0, 1.0);

	glTranslated(translatex*maxDistance*scale, translatey*maxDistance*scale, translatez*maxDistance*scale);

	drawAxis();

	glPushMatrix();
	glScaled(3 * maxDistance / 10, 3 * maxDistance / 10, 3 * maxDistance / 10);
	drawOrigin();
	glPopMatrix();

	glScaled(scale, scale, scale);
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
		drawStream(strm, leftPosition * 170, topPosition*-200, GLUT_BITMAP_HELVETICA_12, 12);
		++leftPosition;
		if (((leftPosition + 1) * 170) > glutGet(GLUT_WINDOW_WIDTH)){
			leftPosition = 0;
			topPosition++;
		}
	}
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

void drawObjects(void) {
	std::lock_guard<std::mutex> lg(*fieldMutex);
	for (const auto i : (*field)->getObjects()) {
		double x, y, z, d;
		char type;
		double Fx, Fy, Fz;
		double Vx , Vy, Vz ,V , recipModVxy;
		double recipGamma;
		std::string name;
		color col;
		{
			std::lock_guard<std::mutex> lg2((*field)->objectsMutex);
			x = i->getX();
			y = i->getY();
			z = i->getZ();
			d = i->getDiameter();
			name = i->getName();
			type = i->getType();
			Vx = i->getVx();
			Vy = i->getVy();
			Vz = i->getVz();
			recipGamma = i->getRecipGamma();
			V = i->getV();
			col = i->getColor();
			if (type == 'r') {
				const rocket* x = dynamic_cast<const rocket*>(i);
				Fx = x->getForceX();
				Fy = x->getForceY();
				Fz = x->getForceZ();
			}
		}
		d = d *0.5;
		glPushMatrix();
		glTranslated(x, y, z);
		if (toggleFloatingDescription) {
			glColor3d(0.0, 0.0, 0.0);
			glPushMatrix();
			glRotated(-rotatez, 0.0, 0.0, 1.0);
			glRotated(-rotatey, 0.0, 1.0, 0.0);
			glRotated(-rotatex, 1.0, 0.0, 0.0);
			glRasterPos3d(0.95*d, 0.95*d, 0.0);
			for (unsigned int i = 0; i < name.length(); i++)
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, name[i]);
			glBegin(GL_LINES);
			glVertex3d(0.75*d, 0.75*d, 0.0);
			glVertex3d(0.9*d, 0.9*d, 0.0);
			glEnd();
			glPopMatrix();
		}
		if (V && (Vx || Vy))
		{
			recipModVxy = 1.0 / sqrt(Vx*Vx + Vy*Vy);
			glRotated(-DEG2RAD*acos(Vz / V), Vy*recipModVxy, -Vx*recipModVxy, 0.0);
		}
		glScaled(1.0, 1.0, recipGamma);
		if (V && (Vx || Vy))
			glRotated(DEG2RAD*acos(Vz / V), Vy*recipModVxy, -Vx*recipModVxy, 0.0);
		glColor3d(col.r, col.g, col.b);
		switch (type){
			case 'o':
				glRotated(90, 1.0, 0.0, 0.0);
				glutWireSphere(d, 40, 20);
				break;
			case 'r':
				glRotated(-DEG2RAD*acos(Fz / sqrt(Fx*Fx + Fy*Fy + Fz*Fz)), Fy, -Fx, 0.0);
				glTranslated(0, 0, -1.558*d);
				glutWireCylinder(d, d*2.3, 40, 20);
				glTranslated(0, 0, d*2.3);
				glutWireCone(d*1.2, d*1.7, 40, 20);
				break;
		}
		glPopMatrix();
	}
}

void display(void) {
	glClearColor(1.0, 1.0, 0.94, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	translateDrawAxisOrigin();
	glColor3d(0.0, 0.0, 0.0);
	if (toggleDescription)
		drawObjectsList();
	if (toggleParameters)
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
	double maxDist = abs(maxX);
	if (abs(maxY) > maxDist)
		maxDist = abs(maxY);
	if (abs(maxZ) > maxDist)
		maxDist = abs(maxZ);
	if (abs(minX) > maxDist)
		maxDist = abs(minX);
	if (abs(minY) > maxDist)
		maxDist = abs(minY);
	if (abs(minZ) > maxDist)
		maxDist = abs(minZ);
	maxDistance = maxDist + maxD;
	_far = 10000 * (maxDist+ maxD) + _near;
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
		(*field)->addMultiplier(1.0);
	}
	break;
	case '-':
	{
		std::lock_guard<std::mutex> lg(*fieldMutex);
		(*field)->addMultiplier(-1.0);
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
}

void specialKeys(int key, int x, int y) {
	double scaleTr = (right - left)*0.03/scale;
	switch (key) {
	case GLUT_KEY_LEFT:
		translatex += scaleTr;
		break;
	case GLUT_KEY_RIGHT:
		translatex -= scaleTr;
		break;
	case GLUT_KEY_UP:
		translatey -= scaleTr;
		break;
	case GLUT_KEY_DOWN:
		translatey += scaleTr;
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
			scale /= 0.87;
		if (button == 4)
			scale *= 0.87;
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
		display();
	}
}

void menu(int value)
{
	switch (value)
	{
	case CENTER_VIEW:
		rotatex = 0.0;
		rotatey = 0.0;
		rotatez = 0.0;
		translatex = 0.0;
		translatey = 0.0;
		translatez = 0.0;
		break;
	case DESCRIPTION_ON_OFF:
		toggleDescription = !toggleDescription;
		break;
	case FLOATING_DESCRIPTION_ON_OFF:
		toggleFloatingDescription = !toggleFloatingDescription;
		break;
	case PARAMETERS_ON_OFF:
		toggleParameters=!toggleParameters;
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
	int menuImage = glutCreateMenu(menu);
	glutAddMenuEntry("Centruj widok", CENTER_VIEW);

	int menuDescription = glutCreateMenu(menu);
	glutAddMenuEntry("Wlacz/Wylacz opisy", DESCRIPTION_ON_OFF);
	glutAddMenuEntry("Wlacz/Wylacz latajace etykiety", FLOATING_DESCRIPTION_ON_OFF);
	glutAddMenuEntry("Wlacz/Wylacz dane symulacji", PARAMETERS_ON_OFF);
	// menu g³ówne

	glutCreateMenu(menu);
	glutAddSubMenu("Obraz", menuImage);
	glutAddSubMenu("Opisy", menuDescription);
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
