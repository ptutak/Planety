#include <iostream>
#include <string>
#include <list>
#include <exception>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <Windows.h>

// CONSTANTS


constexpr double G = 6.6740831e-11;
constexpr double DT = 0.001;

/*

CLASS FLYING OBJECT

*/


class flyingObject
{
	friend class gravityField;
	std::string name;
protected:
	static int oNumber;
	double m;
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

	double getMass(void) const { return m; }
	std::string getName(void) const { return name; }
	double getX(void) const { return x; }
	double getY(void) const { return y; }
	double getZ(void) const { return z; }
	double getVx(void) const { return vx; }
	double getVy(void) const { return vy; }
	double getVz(void) const { return vz; }
	double getAx(void) const { return ax; }
	double getAy(void) const { return ay; }
	double getAz(void) const { return az; }

	virtual void setMass(double mass) { m = mass; }

	flyingObject(std::string oName, double mass = 0.0, double xX = 0.0, double yY = 0.0, double zZ = 0.0, double vX = 0.0, double vY = 0.0, double vZ = 0.0);
	flyingObject(double mass = 0.0, double xX = 0.0, double yY = 0.0, double zZ = 0.0, double vX = 0.0, double vY = 0.0, double vZ = 0.0);
	virtual ~flyingObject(void) {}
};
int flyingObject::oNumber = 0;

flyingObject::flyingObject(std::string oName, double mass, double xX, double yY, double zZ, double vX, double vY, double vZ) :
	name{ oName }, m{ mass }, x{ xX }, y{ yY }, z{ zZ }, vx{ vX }, vy{ vY }, vz{ vZ } {
	oNumber++;
	ax = 0.0;
	ay = 0.0;
	az = 0.0;
}
flyingObject::flyingObject(double mass, double xX, double yY, double zZ, double vX, double vY, double vZ) :
	m{ mass }, x{ xX }, y{ yY }, z{ zZ }, vx{ vX }, vy{ vY }, vz{ vZ } {
	name = "Object" + std::to_string(oNumber);
	oNumber++;
	ax = 0.0;
	ay = 0.0;
	az = 0.0;
}

std::ostream& operator<<(std::ostream& out, const flyingObject *obj) {
	std::cout << std::setprecision(3);
	std::cout << obj->getName() << std::endl;
	std::cout << "m:  " << obj->getMass() << std::endl;
	std::cout << "x:  " << obj->getX() << std::endl;
	std::cout << "y:  " << obj->getY() << std::endl;
	std::cout << "z:  " << obj->getZ() << std::endl;
	std::cout << "Vx: " << obj->getVx() << std::endl;
	std::cout << "Vy: " << obj->getVy() << std::endl;
	std::cout << "Vz: " << obj->getVz() << std::endl;
	std::cout << "ax: " << obj->getAx() << std::endl;
	std::cout << "ay: " << obj->getAy() << std::endl;
	std::cout << "az: " << obj->getAz() << std::endl;

	return std::cout;
}

void flyingObject::updateVelocity(double dt) {
	vx += dt*ax;
	vy += dt*ay;
	vz += dt*az;
}
void flyingObject::updatePosition(double dt) {
	x = x + vx*dt + ax*dt*dt / 2;
	y = y + vy*dt + ay*dt*dt / 2;
	z = z + vz*dt + az*dt*dt / 2;
}
void flyingObject::updateAcceleration(double Ex,double Ey,double Ez) {
	ax = Ex;
	ay = Ey;
	az = Ez;
}

/*

CLASS ROCKET

*/

class rocket :private flyingObject
{
private:
	double Fxe;
	double Fye;
	double Fze;
	double axe;
	double aye;
	double aze;
public:
	rocket(std::string oName, double mass = 0.0, double xX = 0.0, double yY = 0.0, double zZ = 0.0, double vX = 0.0, double vY = 0.0, double vZ = 0.0, double Fx = 0.0, double Fy = 0.0, double Fz = 0.0);
	rocket(double mass = 0.0, double xX = 0.0, double yY = 0.0, double zZ = 0.0, double vX = 0.0, double vY = 0.0, double vZ = 0.0, double Fx = 0.0, double Fy = 0.0, double Fz = 0.0);
	void updateAcceleration(double Ex, double Ey, double Ez);
	void recalculateEngineAcceleration(void);
	void setMass(double mass) {m = mass;recalculateEngineAcceleration();}
};

rocket::rocket(std::string oName, double mass, double xX, double yY, double zZ, double vX, double vY, double vZ, double Fx, double Fy, double Fz) :
	flyingObject(oName, mass, xX, yY, zZ, vX, vY, vZ), Fxe{ Fx }, Fye{ Fy }, Fze{ Fz } {
	if (m != 0.0) {
		axe = Fxe / m;
		aye = Fye / m;
		aze = Fze / m;
	}
	else
	{
		axe = 0.0;
		aye = 0.0;
		aze = 0.0;
	}
}

rocket::rocket(double mass, double xX, double yY, double zZ, double vX, double vY, double vZ, double Fx, double Fy, double Fz) :
	flyingObject(mass, xX, yY, zZ, vX, vY, vZ), Fxe{ Fx }, Fye{ Fy }, Fze{ Fz } {
	if (m != 0.0) {
		axe = Fxe / m;
		aye = Fye / m;
		aze = Fze / m;
	}
	else
	{
		axe = 0.0;
		aye = 0.0;
		aze = 0.0;
	}
}

void rocket::recalculateEngineAcceleration(void) {
	if (m != 0.0) {
		axe = Fxe / m;
		aye = Fye / m;
		aze = Fze / m;
	}
	else
	{
		axe = 0.0;
		aye = 0.0;
		aze = 0.0;
	}
}

void rocket::updateAcceleration(double Ex, double Ey, double Ez) {
	ax = Ex+axe;
	ay = Ey+aye;
	az = Ez+aze;
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
	void removeObject(std::string name);
	void compute(double dt);
	void printObjects(void);
};

void gravityField::printObjects(void) {
	for (auto x : objects) {
		std::cout << x;
	}
}

void gravityField::addObject(flyingObject* next){
	for (auto x : objects) {
		if (x->name == next->name)
			throw std::runtime_error("Name already exists");
	}
	objects.push_back(next);
}

void gravityField::removeObject(std::string name) {
	std::list<flyingObject*>::const_iterator pos;

	for (pos = objects.cbegin(); pos != objects.cend(); pos++) {
		if ((*pos)->name == name) {
			flyingObject* tmp;
			tmp = (*pos);
			objects.erase(pos);
			delete tmp;
			break;
		}
	}
}
void gravityField::compute(double dt) {
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
					std::cout <<"iDisj " <<iDisj << std::endl;
					std::cout << "iDisjSq " << iDisjSq << std::endl;
					std::cout << "iDisj*iDisjSq" << (iDisjSq * iDisj) << std::endl;
					std::cout << "MGRRR"<< MGRRR << std::endl;
*/
					Ex += (j->x - i->x) * MGRRR; //direction j-i
					Ey += (j->y - i->y) * MGRRR;
					Ez += (j->z - i->z) * MGRRR;

//					std::cout << Ex << std::endl;
				
				}
			}
		}
		i->updateAcceleration(Ex, Ey, Ez);
		i->updateVelocity(dt);
		i->updatePosition(dt);
	}
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
		test.compute(static_cast<double>(dif)/static_cast<double>(CLOCKS_PER_SEC));
		cout << dif;

		dif = clock()-start_clock;
		start_clock = clock();
	}
	system("pause");
}