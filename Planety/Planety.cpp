#include <iostream>
#include <cstdlib>
#include <string>
#include <list>
#include <stdexcept>
#include <exception>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <Windows.h>
#include <sstream>

// CONSTANTS

constexpr double G = 6.6740831e-11;


/*

CLASS FLYING OBJECT

*/


class flyingObject
{
	friend class gravityField;
protected:
	static int oNumber;
	char type;
	std::string name;
	double m;
	double d;
	double x;
	double y;
	double z;
	double vx;
	double vy;
	double vz;
	double ax;
	double ay;
	double az;
	double xSpin;
	double ySpin;
	double zSpin;
public:
	double distance(const flyingObject& obj) const { return sqrt((obj.x - x)*(obj.x - x) + (obj.y - y)*(obj.y - y) + (obj.z - z)*(obj.z - z)); }
	double distanceSquared(const flyingObject& obj) const { return (obj.x - x)*(obj.x - x) + (obj.y - y)*(obj.y - y) + (obj.z - z)*(obj.z - z); }
	virtual void updateAcceleration(double Ex, double Ey, double Ez);
	virtual void updatePosition(double dt);
	virtual void updateVelocity(double dt);
	virtual std::string shortDescription(void);

	std::string getName(void) const { return name; }
	double getMass(void) const { return m; }
	double getDiameter(void) const { return d; }
	double getX(void) const { return x; }
	double getY(void) const { return y; }
	double getZ(void) const { return z; }
	double getVx(void) const { return vx; }
	double getVy(void) const { return vy; }
	double getVz(void) const { return vz; }
	double getAx(void) const { return ax; }
	double getAy(void) const { return ay; }
	double getAz(void) const { return az; }

	void setName(std::string newName) { name = newName; }
	virtual void setMass(double mass) { m = mass; }
	void setDiameter(double diameter) { d = diameter; }
	void setX(double xX) { x = xX; }
	void setY(double yY) { y = yY; }
	void setZ(double zZ) { z = zZ; }
	void setVx(double Vx) { vx = Vx; }
	void setVy(double Vy) { vy = Vy; }
	void setVz(double Vz) { vx = Vz; }


	flyingObject(std::string oName, double mass = 0.0,double diameter=0.0, double xX = 0.0, double yY = 0.0, double zZ = 0.0, double vX = 0.0, double vY = 0.0, double vZ = 0.0);
	flyingObject(double mass = 0.0,double diameter=0.0, double xX = 0.0, double yY = 0.0, double zZ = 0.0, double vX = 0.0, double vY = 0.0, double vZ = 0.0);
	virtual ~flyingObject(void) {}
};

int flyingObject::oNumber = 0;

std::ostream& operator<<(std::ostream& out, const flyingObject obj) {
	std::streamsize fp = std::cout.precision();
	std::cout << std::setprecision(3);
	std::cout << obj.getName() << std::endl;
	std::cout << "m:  " << obj.getMass() << std::endl;
	std::cout << "d:  " << obj.getDiameter() << std::endl;
	std::cout << "x:  " << obj.getX() << std::endl;
	std::cout << "y:  " << obj.getY() << std::endl;
	std::cout << "z:  " << obj.getZ() << std::endl;
	std::cout << "Vx: " << obj.getVx() << std::endl;
	std::cout << "Vy: " << obj.getVy() << std::endl;
	std::cout << "Vz: " << obj.getVz() << std::endl;
	std::cout << "ax: " << obj.getAx() << std::endl;
	std::cout << "ay: " << obj.getAy() << std::endl;
	std::cout << "az: " << obj.getAz() << std::endl;
	std::cout << std::setprecision(fp);
	return std::cout;
}

std::string flyingObject::shortDescription(void) {
	std::string tmp;
	tmp =name + "," + std::to_string(m)+","+std::to_string(d) + "," + std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(z) + "," + std::to_string(vx) + "," + std::to_string(vy) + "," + std::to_string(vz);
	return tmp;
}
void flyingObject::updateAcceleration(double Ex, double Ey, double Ez) {
	ax = Ex;
	ay = Ey;
	az = Ez;
}
void flyingObject::updatePosition(double dt) {
	x = x + vx*dt + ax*dt*dt / 2;
	y = y + vy*dt + ay*dt*dt / 2;
	z = z + vz*dt + az*dt*dt / 2;
}
void flyingObject::updateVelocity(double dt) {
	vx += dt*ax;
	vy += dt*ay;
	vz += dt*az;
}
flyingObject::flyingObject(std::string oName, double mass, double diameter, double xX, double yY, double zZ, double vX, double vY, double vZ) :
	name{ oName }, type{ 'o' }, m{ mass }, d{ diameter }, x{ xX }, y{ yY }, z{ zZ }, vx{ vX }, vy{ vY }, vz{ vZ } {
	oNumber++;
	ax = 0.0;
	ay = 0.0;
	az = 0.0;
}
flyingObject::flyingObject(double mass, double diameter, double xX, double yY, double zZ, double vX, double vY, double vZ) :
	m{ mass }, type{ 'o' }, d{ diameter }, x{ xX }, y{ yY }, z{ zZ }, vx{ vX }, vy{ vY }, vz{ vZ } {
	name = "Object" + std::to_string(oNumber);
	oNumber++;
	ax = 0.0;
	ay = 0.0;
	az = 0.0;
}

/*

CLASS ROCKET

*/

class rocket :public flyingObject
{
private:
	double Fxe;
	double Fye;
	double Fze;
	double axe;
	double aye;
	double aze;
public:
	void recalculateEngineAcceleration(void);
	std::string shortDescription(void);
	void updateAcceleration(double Ex, double Ey, double Ez);

	double getForceX(void) const { return Fxe; }
	double getForceY(void) const { return Fye; }
	double getForceZ(void) const { return Fze; }

	void setMass(double mass) { m = mass; recalculateEngineAcceleration(); }
	void setForceX(double Fx) { Fxe = Fx; }
	void setForceY(double Fy) { Fye = Fy; }
	void setForceZ(double Fz) { Fze = Fz; }
		
	rocket(std::string oName, double mass = 0.0,double diameter=0.0, double xX = 0.0, double yY = 0.0, double zZ = 0.0, double vX = 0.0, double vY = 0.0, double vZ = 0.0, double Fx = 0.0, double Fy = 0.0, double Fz = 0.0);
	rocket(double mass = 0.0,double diameter=0.0, double xX = 0.0, double yY = 0.0, double zZ = 0.0, double vX = 0.0, double vY = 0.0, double vZ = 0.0, double Fx = 0.0, double Fy = 0.0, double Fz = 0.0);
	rocket(const flyingObject& obj, double Fx = 0.0, double Fy = 0.0, double Fz = 0.0);
	~rocket(void) {}
};


void rocket::recalculateEngineAcceleration(void) {
	if (m != 0.0) {
		axe = Fxe / m;
		aye = Fye / m;
		aze = Fze / m;
	}
	else {
		axe = 0.0;
		aye = 0.0;
		aze = 0.0;
	}
}

std::string rocket::shortDescription(void) {
	std::string tmp;
	tmp = name + "," + std::to_string(m) + "," + std::to_string(d) + "," + std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(z) + "," + std::to_string(vx) + "," + std::to_string(vy) + "," + std::to_string(vz) + "," + std::to_string(Fxe) + "," + std::to_string(Fye) + "," + std::to_string(Fze);
	return tmp;
}

void rocket::updateAcceleration(double Ex, double Ey, double Ez) {
	ax = Ex+axe;
	ay = Ey+aye;
	az = Ez+aze;
}
rocket::rocket(const flyingObject& obj, double Fx, double Fy, double Fz) :
	flyingObject{ obj }, Fxe{ Fx }, Fye{ Fy }, Fze{ Fz } {
	type = 'r';
	recalculateEngineAcceleration();
}

rocket::rocket(std::string oName, double mass, double diameter, double xX, double yY, double zZ, double vX, double vY, double vZ, double Fx, double Fy, double Fz) :
	flyingObject(oName, mass, diameter, xX, yY, zZ, vX, vY, vZ), Fxe{ Fx }, Fye{ Fy }, Fze{ Fz } {
	type = 'r';
	recalculateEngineAcceleration();
}

rocket::rocket(double mass, double diameter, double xX, double yY, double zZ, double vX, double vY, double vZ, double Fx, double Fy, double Fz) :
	flyingObject(mass, diameter, xX, yY, zZ, vX, vY, vZ), Fxe{ Fx }, Fye{ Fy }, Fze{ Fz } {
	type = 'r';
	recalculateEngineAcceleration();
}
/*

CLASS GRAVITY FIELD

*/

class gravityField
{
private:
	std::list<flyingObject*> objects;
public:
	void addObject(flyingObject* next) throw (std::invalid_argument);
	void computeGravity(double dt);
	void printObjects(void);
	void printObjectsList(void);
	void removeObject(const std::string name);
	bool searchObject(const std::string name);
	
	~gravityField(void);
};

void gravityField::addObject(flyingObject* next){
	for (auto x : objects) {
		if (x->name == next->name)
			throw std::invalid_argument("Obiekt o tej nazwie ju¿ istnieje");
	}
	objects.push_back(next);
}
void gravityField::computeGravity(double dt) {
	for (auto i : objects) {
		double Ex = 0.0;
		double Ey = 0.0;
		double Ez = 0.0;
		for (auto j : objects) {
			if (i == j)
				continue;
			else {
				double iDisj = i->distance(*j);
				if (iDisj) {
					double iDisjSq = i->distanceSquared(*j);
					double MGRRR = G*j->m / (iDisjSq * iDisj);
					/*
					Debug option:
					std::cout <<"iDisj " <<iDisj << std::endl;
					std::cout << "iDisjSq " << iDisjSq << std::endl;
					std::cout << "iDisj*iDisjSq" << (iDisjSq * iDisj) << std::endl;
					std::cout << "MGRRR"<< MGRRR << std::endl;
					*/
					Ex += (j->x - i->x) * MGRRR; //direction j-i
					Ey += (j->y - i->y) * MGRRR;
					Ez += (j->z - i->z) * MGRRR;
				}
			}
		}
		i->updateAcceleration(Ex, Ey, Ez);
		i->updatePosition(dt);
		i->updateVelocity(dt);
	}
}
void gravityField::printObjects(void) {
	for (auto x : objects) {
		std::cout << *x;
	}
}
void gravityField::printObjectsList(void) {
	for (auto x : objects) {
		std::cout << x->shortDescription();
		std::cout << std::endl;
	}
}
void gravityField::removeObject(const std::string name) {
	std::list<flyingObject*>::const_iterator pos;
	for (pos = objects.cbegin(); pos != objects.cend(); pos++) {
		if ((*pos)->name == name) {
			flyingObject* tmp=*pos;
			objects.erase(pos);
			delete tmp;
			break;
		}
	}
}
bool gravityField::searchObject(const std::string name) {
	for (auto x : objects) {
		if (x->getName() == name)
			return true;
	}
	return false;
}
gravityField::~gravityField(void) {
	while (!objects.empty()) {
		delete objects.front();
		objects.pop_front();
	}
}

/*

INPUT/OUTPUT FUNCTIONS

*/

flyingObject* readLineFromStream(std::istream &in, gravityField* gravField) {
	flyingObject* tmpObject = nullptr;
	std::string line;
	std::getline(in, line);
	std::stringstream linestrm(line);
	line = "";
	tmpObject = new flyingObject;
	std::getline(linestrm, line, ',');
	if (line != "")
		tmpObject->setName(line);
	if (!linestrm.eof()) {
		std::string line;
		std::getline(linestrm, line, ',');
		double number = 0.0;
		std::stringstream data(line);
		data >> number;
		if (data.good())
			tmpObject->setMass(number);
	}
	if (!linestrm.eof()) {
		std::string line;
		std::getline(linestrm, line, ',');
		double number = 0.0;
		std::stringstream data(line);
		data >> number;
		if (number != 0.0)
			tmpObject->setDiameter(number);
	}
	if (!linestrm.eof()) {
		std::string line;
		std::getline(linestrm, line, ',');
		double number = 0.0;
		std::stringstream data(line);
		data >> number;
		if (number != 0.0)
			tmpObject->setX(number);
	}
	if (!linestrm.eof()) {
		std::string line;
		std::getline(linestrm, line, ',');
		double number = 0.0;
		std::stringstream data(line);
		data >> number;
		if (number != 0.0)
			tmpObject->setY(number);
	}
	if (!linestrm.eof()) {
		std::string line;
		std::getline(linestrm, line, ',');
		double number = 0.0;
		std::stringstream data(line);
		data >> number;
		if (number != 0.0)
			tmpObject->setZ(number);
	}
	if (!linestrm.eof()) {
		std::string line;
		std::getline(linestrm, line, ',');
		double number = 0.0;
		std::stringstream data(line);
		data >> number;
		if (number != 0.0)
			tmpObject->setVx(number);
	}
	if (!linestrm.eof()) {
		std::string line;
		std::getline(linestrm, line, ',');
		double number = 0.0;
		std::stringstream data(line);
		data >> number;
		if (number != 0.0)
			tmpObject->setVy(number);
	}
	if (!linestrm.eof()) {
		std::string line;
		std::getline(linestrm, line, ',');
		double number = 0.0;
		std::stringstream data(line);
		data >> number;
		if (number != 0.0)
			tmpObject->setVz(number);
	}
	if (!linestrm.eof()) {
		std::string line;
		rocket* tmpRocket = new rocket{ *tmpObject };
		std::getline(linestrm, line, ',');
		double number = 0.0;
		std::stringstream data(line);
		data >> number;
		if (number != 0.0)
			tmpRocket->setForceX(number);
		if (!linestrm.eof()) {
			std::string line;
			std::getline(linestrm, line, ',');
			double number = 0.0;
			std::stringstream data(line);
			data >> number;
			if (number != 0.0)
				tmpRocket->setForceY(number);
		}
		if (!linestrm.eof()) {
			std::string line;
			std::getline(linestrm, line, ',');
			double number = 0.0;
			std::stringstream data;
			data >> number;
			if (number != 0.0)
				tmpRocket->setForceZ(number);
		}
		delete tmpObject;
		tmpObject = dynamic_cast<flyingObject*>(tmpRocket);
	}
	return tmpObject;
}

/*

MENU - FUNCTIONS

*/

void addObjectMenu(gravityField* gravField) {
	char choice;
	do {
		std::cout << "0 - Zwyk³y obiekt" << std::endl;
		std::cout << "1 - Rakieta" << std::endl;
		std::cout << "9 - WyjdŸ" << std::endl;
		std::cin >> choice;
	} while (choice != '0' && choice != '1' && choice != '9');
	switch (choice) {
	case '0':
		std::cout << "Podaj dane obiektu w formacie:" << std::endl;
		std::cout << "[Nazwa][,Masa[,Œrednica[,x[,y[,z[,Vx[,Vy[,Vz]]]]]]]]" << std::endl;
		{
			flyingObject* made = readLineFromStream(std::cin, gravField);
			if (made) {
				try {
					gravField->addObject(made);
				}
				catch (const std::invalid_argument exc) {
					std::cout << exc.what() << std::endl;
					delete made;
					made = nullptr;
				}
				if (made)
					std::cout << made->shortDescription();
			}
		}
		break;
	case '1':
		std::cout << "Podaj dane obiektu w formacie:" << std::endl;
		std::cout << "[Nazwa][,Masa[,Œrednica[,x[,y[,z[,Vx[,Vy[,Vz[,Fx[,Fy[,Fz]]]]]]]]]]]" << std::endl;
		{
			flyingObject* made = readLineFromStream(std::cin, gravField);
			if (made) {
				try {
					gravField->addObject(made);
				}
				catch (const std::invalid_argument exc) {
					std::cout << exc.what() << std::endl;
					delete made;
					made = nullptr;
				}
				if (made)
					std::cout << made->shortDescription();
			}
		}
		break;
	case '9':
		return;
	}
	
}

void deleteObjectMenu(gravityField* gravField) {
	std::cout << "Lista obiektów:" << std::endl;
	std::cout << "Nazwa,Masa,Œrednica,x,y,z,Vx,Vy,Vz[,Fx,Fy,Fz]" << std::endl;
	gravField->printObjectsList();
	std::cout << "Podaj nazwê obiektu, który chcesz usun¹æ, lub Esc i Enter by wyjœæ" << std::endl;
	char c;
	c = std::cin.get();
	if (static_cast<int>(c) == 27)
		return;
	string name;
	std::cin >> name;
	name = c + name;
	if (gravField->searchObject(name))
		gravField->removeObject(name);
	else
		std::cout << "B³êdna nazwa obiektu" << std::endl;
}

void modifyObjectMenu(gravityField* gravField) {
	std::cout << "Lista obiektów:" << std::endl;
	std::cout << "Nazwa,Masa,Œrednica,x,y,z,Vx,Vy,Vz[,Fx,Fy,Fz]" << std::endl;
	gravField->printObjectsList();
	std::cout << "Podaj nazwê obiektu, który chcesz zmodyfikowaæ, lub Esc i Enter by wyjœæ" << std::endl;
	char c;
	c = std::cin.get();
	if (static_cast<int>(c) == 27)
		return;
	string name;
	std::cin >> name;
	name = c + name;
	if (gravField->searchObject(name)) {
		std::cout << "Podaj nowe dane dla obiektu " + name + " w formacie: [Nazwa][,Masa[,Œrednica[,x[,y[,z[,Vx[,Vy[,Vz[,Fx[,Fy[,Fz]]]]]]]]]]]" << std::endl;
		flyingObject* tmpObject = readLineFromStream(std::cin, gravField);
		gravField->removeObject(name);
		gravField->addObject(tmpObject);
	}
	else
		std::cout << "B³êdna nazwa obiektu" << std::endl;
}
/*

INIT

*/

int init(void) {
	gravityField* gravField = new gravityField;
	char choice;
	std::cout << "Witaj w programie Planety:" << std::endl;
	do {
		do {
			std::cout << "0 - Dodaj obiekt" << std::endl;
			std::cout << "1 - Usuñ obiekt" << std::endl;
			std::cout << "2 - Modyfikuj obiekt" << std::endl;
			std::cout << "3 - Wyœwietl obiekty" << std::endl;
			std::cout << "4 - Rozpocznij symulacjê" << std::endl;
			std::cout << "5 - Zapisz konfiguracjê do pliku" << std::endl;
			std::cout << "6 - Wczytaj konfiguracjê z pliku" << std::endl;
			std::cout << "9 - WyjdŸ" << std::endl;
			cin >> choice;
		} while (choice < '0' || choice > '9' || choice == '7' || choice == '8');
		
		switch (choice) {
		case '0':
			addObjectMenu(gravField);
			break;
		case '1':
			deleteObjectMenu(gravField);
			break;
		case '2':
			modifyObjectMenu(gravField);
			break;
		case '3':
			break;
		case '4':
			break;
		case '5':
			break;
		case '6':
			break;
		case '9':
			std::cout << "Goodbye" << std::endl;
			return 0;
		}
	} while (1);
}

/*

MAIN

*/

using namespace std;
int main(void)
{
	clock_t start_clock;
	clock_t dif=15;
	gravityField test;
	test.addObject(new flyingObject{ 6.0e24 });
	test.addObject(new flyingObject{ 1.0,6.4e6,0,0,0,7910.0 });
	start_clock = clock();
	while (1) {
		
		system("cls");
		test.printObjects();
		test.computeGravity(static_cast<double>(dif)/static_cast<double>(CLOCKS_PER_SEC));
		cout << dif;

		dif = clock()-start_clock;
		start_clock = clock();
	}
	system("pause");
}