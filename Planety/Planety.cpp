#include <iostream>
#include <fstream>
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
#include <conio.h>

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
	virtual std::string shortDescription(int precision=-1);

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
	out << std::setprecision(3);
	out << std::fixed;
	out << obj.getName() << std::endl;
	out << "m:  " << obj.getMass() << std::endl;
	out << "d:  " << obj.getDiameter() << std::endl;
	out << "x:  " << obj.getX() << std::endl;
	out << "y:  " << obj.getY() << std::endl;
	out << "z:  " << obj.getZ() << std::endl;
	out << "Vx: " << obj.getVx() << std::endl;
	out << "Vy: " << obj.getVy() << std::endl;
	out << "Vz: " << obj.getVz() << std::endl;
	out << "ax: " << obj.getAx() << std::endl;
	out << "ay: " << obj.getAy() << std::endl;
	out << "az: " << obj.getAz() << std::endl;
	out << std::defaultfloat;
	return out;
}

std::string flyingObject::shortDescription(int prec) {
	std::string retString, tempString;
	std::stringstream strm;
	retString = name;
	if (prec >= 0)
		strm << std::setprecision(prec);
	{ strm.str(""); strm << m; tempString = strm.str(); retString += ',' + tempString; }
	{ strm.str(""); strm.clear(); strm << d; tempString = strm.str(); retString += ',' + tempString; }
	{ strm.str(""); strm.clear(); strm << x; tempString = strm.str(); retString += ',' + tempString; }
	{ strm.str(""); strm.clear(); strm << y; tempString = strm.str(); retString += ',' + tempString; }
	{ strm.str(""); strm.clear(); strm << z; tempString = strm.str(); retString += ',' + tempString; }
	{ strm.str(""); strm.clear(); strm << vx; tempString = strm.str(); retString += ',' + tempString; }
	{ strm.str(""); strm.clear(); strm << vy; tempString = strm.str(); retString += ',' + tempString; }
	{ strm.str(""); strm.clear(); strm << vz; tempString = strm.str(); retString += ',' + tempString; }
	return retString;
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
	std::string shortDescription(int precision=-1);
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

std::string rocket::shortDescription(int prec) {
	std::string retString, tempString;
	std::stringstream strm;
	retString = name;
	if (prec >= 0)
		strm << std::setprecision(prec);
	{ strm.str(""); strm << m; tempString = strm.str(); retString += ',' + tempString; }
	{ strm.str(""); strm.clear(); strm << d; tempString = strm.str(); retString += ',' + tempString; }
	{ strm.str(""); strm.clear(); strm << x; tempString = strm.str(); retString += ',' + tempString; }
	{ strm.str(""); strm.clear(); strm << y; tempString = strm.str(); retString += ',' + tempString; }
	{ strm.str(""); strm.clear(); strm << z; tempString = strm.str(); retString += ',' + tempString; }
	{ strm.str(""); strm.clear(); strm << vx; tempString = strm.str(); retString += ',' + tempString; }
	{ strm.str(""); strm.clear(); strm << vy; tempString = strm.str(); retString += ',' + tempString; }
	{ strm.str(""); strm.clear(); strm << vz; tempString = strm.str(); retString += ',' + tempString; }
	{ strm.str(""); strm.clear(); strm << Fxe; tempString = strm.str(); retString += ',' + tempString; }
	{ strm.str(""); strm.clear(); strm << Fye; tempString = strm.str(); retString += ',' + tempString; }
	{ strm.str(""); strm.clear(); strm << Fze; tempString = strm.str(); retString += ',' + tempString; }
	return retString;
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
	void addObject(flyingObject* next);
	void computeGravity(double dt);
	void printObjects(void) const;
	void printObjectsList(std::ostream& out,int precision=-1) const;
	void removeObject(const std::string name);
	bool searchObject(const std::string name) const;
	
	~gravityField(void);
};

void gravityField::addObject(flyingObject* next){
	for (auto x : objects) {
		if (x->name == next->name)
			throw std::invalid_argument("Obiekt o nazwie "+next->getName()+" juz istnieje");
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
void gravityField::printObjects(void) const {
	for (auto x : objects) {
		std::cout << *x;
	}
}
void gravityField::printObjectsList(std::ostream& out,int prec) const {
	for (auto x : objects) {
		out << x->shortDescription(prec);
		out << std::endl;
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
bool gravityField::searchObject(const std::string name) const {
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

flyingObject* readObjectFromStream(std::istream& in) {
	flyingObject* tmpObject = nullptr;
	std::string line;
	std::getline(in, line);
	if (line != "") {
		tmpObject = new flyingObject;
		std::stringstream linestrm(line);
		line = "";
		std::getline(linestrm, line, ',');
		if (line!="")
			tmpObject->setName(line);
		if (!linestrm.eof()) {
			std::string line;
			std::getline(linestrm, line, ',');
			double number = 0.0;
			std::stringstream data(line);
			data >> number;
			if (number!=0.0)
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
	}
	return tmpObject;
}

/*

MENU - FUNCTIONS

*/

void addObjectMenu(gravityField* gravField) {
	std::cout << std::endl;
	std::cout << "Podaj dane obiektu w formacie:" << std::endl;
	std::cout << "[Nazwa][,Masa[,Srednica[,x[,y[,z[,Vx[,Vy[,Vz[,Fx[,Fy[,Fz]]]]]]]]]]]" << std::endl;
	flyingObject* made = readObjectFromStream(std::cin);
	if (made) {
		try {
			gravField->addObject(made);
		}
		catch (const std::invalid_argument exc) {
			std::cout << exc.what() << std::endl;
			delete made;
			made = nullptr;
		}
		if (made) {
			std::cout << std::endl;
			std::cout << "Dodano obiekt: " << made->shortDescription() << std::endl;
		}
	}
	else
		std::cout << "Nic nie dodalem" << std::endl;
}

void deleteObjectMenu(gravityField* gravField) {
	std::cout << std::endl;
	std::cout << "Lista obiektow:" << std::endl;
	std::cout << "Nazwa,Masa,Srednica,x,y,z,Vx,Vy,Vz[,Fx,Fy,Fz]" << std::endl;
	std::cout << std::endl;
	gravField->printObjectsList(std::cout);
	std::cout << std::endl;
	std::cout << "Podaj nazwe obiektu, ktory chcesz usunac, lub nacisnij Enter by wyjsc" << std::endl;
	std::string name;
	std::getline(std::cin, name);
	if (name == "")
		return;
	if (gravField->searchObject(name))
		gravField->removeObject(name);
	else
		std::cout << "Bledna nazwa obiektu" << std::endl;
}

void modifyObjectMenu(gravityField* gravField) {
	std::cout << std::endl;
	std::cout << "Lista obiektow:" << std::endl;
	std::cout << "Nazwa,Masa,Srednica,x,y,z,Vx,Vy,Vz[,Fx,Fy,Fz]" << std::endl;
	std::cout << std::endl;
	gravField->printObjectsList(std::cout);
	std::cout << std::endl;
	std::cout << "Podaj nazwe obiektu, ktory chcesz zmodyfikowac, lub Enter by wyjsc" << std::endl;
	std::string name;
	std::getline(std::cin, name);
	if (name == "")
		return;
	if (gravField->searchObject(name)) {
		std::cout << "Podaj nowe dane dla obiektu " + name + " w formacie: [Nazwa][,Masa[,Srednica[,x[,y[,z[,Vx[,Vy[,Vz[,Fx[,Fy[,Fz]]]]]]]]]]]" << std::endl;
		flyingObject* tmpObject = readObjectFromStream(std::cin);
		if (gravField->searchObject(tmpObject->getName())&&(tmpObject->getName()!=name))
		{
			std::cout << "Nazwa obiektu juz istnieje";
			delete tmpObject;
			return;
		}
		gravField->removeObject(name);
		gravField->addObject(tmpObject);
		std::cout << "Zrobione" << std::endl;
	}
	else
	{
		std::cout << "Bledna nazwa obiektu" << std::endl;
	}
}

void startSimulationMenu(gravityField* gravField) {
	clock_t start_clock;
	clock_t dif = 15;
	start_clock = clock();
	system("cls");
	std::cout << "Nacisnij Enter, by rozpoczac symulacje, podczas symulacji nacisnij Escape by przerwac." << std::endl;
	system("pause");
	while (!(GetAsyncKeyState(VK_ESCAPE))) {
		system("cls");
		gravField->printObjects();
		gravField->computeGravity(static_cast<double>(dif) / static_cast<double>(CLOCKS_PER_SEC));
		std::cout << dif;
		do
			dif = clock() - start_clock;
		while (dif < 100);//kontrola czasu - obliczanie czasu ostatniej iteracji
		start_clock = clock();			//pocz¹tek liczenia czasu kolejnej iteracji
	}
}

void saveToFileMenu(const gravityField* gravField) {
	std::string filename="";
	std::cout << std::endl;
	std::cout << "Podaj nazwe pliku" << std::endl;
	std::getline(std::cin, filename);
	std::ofstream file;
	file.open(filename, std::fstream::out|std::fstream::trunc);
	if (file.good()) {
		gravField->printObjectsList(file,11);
		file.close();
	}
	else {
		std::cout << "Bledna nazwa pliku" << std::endl;
	}
}

void readFromFileMenu(gravityField* gravField) {
	std::string filename;
	std::cout << std::endl;
	std::cout << "Podaj nazwe pliku" << std::endl;
	std::getline(std::cin, filename);
	std::ifstream file;
	file.open(filename, std::fstream::in);
	if (file.good()) {
		while (!file.eof()) {
			flyingObject* tmpObject = readObjectFromStream(file);
			if (tmpObject)
			{
				try {
					gravField->addObject(tmpObject);
				}
				catch (std::invalid_argument x) {
					std::cout << x.what() << std::endl;
					delete tmpObject;
					tmpObject = nullptr;
				}
			}
			if (tmpObject)
				std::cout << "Dodano obiekt: " << tmpObject->shortDescription() << std::endl;
		}
		file.close();
	}
	else
	{
		std::cout << "Niepoprawna nazwa pliku" << std::endl;
	}

}

void deleteAllObjectsMenu(gravityField*& gravField) {
	std::cout << std::endl;
	std::cout << "Czy na pewno chcesz usunac wszystkie obiekty? [tT/cokolwiek]" << std::endl;
	char c;
	std::string tmp;
	std::cin >> c;
	std::getline(std::cin, tmp);
	if (c == 'T' || c=='t') {
			delete gravField;
			gravField = new gravityField;
			std::cout << "OK - usunieto" << std::endl;
			return;
	}
	std::cout << "Nie usuwam" << std::endl;
}

void developerModeMenu(gravityField* gravField) {
	std::cout << "Podaj czas obliczen w s:" << std::endl;
	double time;
	std::cin >> time;
	std::cout << "Podaj wielkosc ramki w ms:" << std::endl;
	int frame;
	std::cin >> frame;
	double currentTime = 0.0;
	double newFrame = frame / static_cast<double>(1000);
	while (currentTime < time) {
		gravField->computeGravity(newFrame);
		currentTime += newFrame;
	}
	gravField->printObjects();
	std::string tmp;
	std::getline(std::cin, tmp);
}
/*

INIT

*/

int init(void) {
	gravityField* gravField = new gravityField;
	char choice;
	std::string tmp;
	std::cout << "Witaj w programie Planety:" << std::endl;
	do {
		do {
			std::cout << std::endl;
			std::cout << "0 - Dodaj obiekt" << std::endl;
			std::cout << "1 - Usun obiekt" << std::endl;
			std::cout << "2 - Modyfikuj obiekt" << std::endl;
			std::cout << "3 - Wyswietl obiekty" << std::endl;
			std::cout << "4 - Rozpocznij symulacje" << std::endl;
			std::cout << "5 - Zapisz konfiguracje do pliku" << std::endl;
			std::cout << "6 - Wczytaj konfiguracje z pliku" << std::endl;
			std::cout << "7 - Wyczysc obiekty" << std::endl;
			std::cout << "8 - Deweloper mode" << std::endl;
			std::cout << "9 - Wyjdz" << std::endl;
			choice = std::cin.peek();
			std::getline(std::cin, tmp);
		} while (choice < '0' || choice > '9');
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
			std::cout << std::endl;
			std::cout << "Lista obiektow:" << std::endl;
			std::cout << "Nazwa,Masa,Srednica,x,y,z,Vx,Vy,Vz[,Fx,Fy,Fz]" << std::endl;
			std::cout << std::endl;
			gravField->printObjectsList(std::cout);
			break;
		case '4':
			startSimulationMenu(gravField);
			break;
		case '5':
			saveToFileMenu(gravField);
			break;
		case '6':
			readFromFileMenu(gravField);
			break;
		case '7':
			deleteAllObjectsMenu(gravField);
			break;
		case '8':
			developerModeMenu(gravField);
			break;
		case '9':
			delete gravField;
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
//	init();
	clock_t t,time=0;
	std::cout << "start" << std::endl;
	system("pause");
	clock_t start = clock();
	while (1)
	{
		do
			t = clock()-start;
		while (t < 100);
		start = clock();
		system("cls");
		time += t;
		std::cout << time / static_cast<double>(CLOCKS_PER_SEC) << std::endl;
	}
	system("pause");
}
