#include "PlanetyClasses.h"


//FRAME INFO
simulationInfo info;

simulationInfo& getInfo(void) { return info; }

int simulationInfo::getLastFrame(void) {
	std::lock_guard<std::mutex> lg(frameMutex);
	return lastFrame;
}
double simulationInfo::getRealTime(void) {
	std::lock_guard<std::mutex> lg(realTimeMutex);
	return ((clock() - realClock)/static_cast<double>(CLOCKS_PER_SEC));
}
void simulationInfo::setLastFrame(int fr) {
	std::lock_guard<std::mutex> lg(frameMutex);
	lastFrame = fr;
}
void simulationInfo::setRealClock() {
	std::lock_guard<std::mutex> lg(realTimeMutex);
	realClock = clock();
}

/*

CLASS FLYING OBJECT

*/

int flyingObject::oNumber = 0;

std::ostream& operator<<(std::ostream& out, const flyingObject obj) {
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
	return out;
}

std::string flyingObject::shortDescription(int prec) const {
	std::string retString, tempString;
	std::stringstream strm;
	retString = name;
	if (prec >= 0)
		strm << std::setprecision(prec);
	strm.str(""); strm << m; tempString = strm.str(); retString += ',' + tempString;
	strm.str(""); strm.clear(); strm << d; tempString = strm.str(); retString += ',' + tempString;
	strm.str(""); strm.clear(); strm << x; tempString = strm.str(); retString += ',' + tempString;
	strm.str(""); strm.clear(); strm << y; tempString = strm.str(); retString += ',' + tempString;
	strm.str(""); strm.clear(); strm << z; tempString = strm.str(); retString += ',' + tempString;
	strm.str(""); strm.clear(); strm << vx; tempString = strm.str(); retString += ',' + tempString;
	strm.str(""); strm.clear(); strm << vy; tempString = strm.str(); retString += ',' + tempString;
	strm.str(""); strm.clear(); strm << vz; tempString = strm.str(); retString += ',' + tempString;
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

std::string rocket::shortDescription(int prec) const {
	std::string retString, tempString;
	std::stringstream strm;
	retString = name;
	if (prec >= 0)
		strm << std::setprecision(prec);
	strm.str(""); strm << m; tempString = strm.str(); retString += ',' + tempString;
	strm.str(""); strm.clear(); strm << d; tempString = strm.str(); retString += ',' + tempString;
	strm.str(""); strm.clear(); strm << x; tempString = strm.str(); retString += ',' + tempString;
	strm.str(""); strm.clear(); strm << y; tempString = strm.str(); retString += ',' + tempString;
	strm.str(""); strm.clear(); strm << z; tempString = strm.str(); retString += ',' + tempString;
	strm.str(""); strm.clear(); strm << vx; tempString = strm.str(); retString += ',' + tempString;
	strm.str(""); strm.clear(); strm << vy; tempString = strm.str(); retString += ',' + tempString;
	strm.str(""); strm.clear(); strm << vz; tempString = strm.str(); retString += ',' + tempString;
	strm.str(""); strm.clear(); strm << Fxe; tempString = strm.str(); retString += ',' + tempString;
	strm.str(""); strm.clear(); strm << Fye; tempString = strm.str(); retString += ',' + tempString;
	strm.str(""); strm.clear(); strm << Fze; tempString = strm.str(); retString += ',' + tempString;
	return retString;
}

void rocket::updateAcceleration(double Ex, double Ey, double Ez) {
	ax = Ex + axe;
	ay = Ey + aye;
	az = Ez + aze;
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
void gravityField::addMultiplier(double add) { 
	std::lock_guard<std::mutex> lg(multiplierMutex);
	timeMultiplier += add;
	if (timeMultiplier < 0.0)
		timeMultiplier = 0.0;
}


void gravityField::addObject(flyingObject* next) {
	{
		std::lock_guard<std::mutex> lg(objectsMutex);
		for (auto x : objects) {
			if (x->name == next->name)
				throw std::invalid_argument("Obiekt o nazwie " + next->getName() + " juz istnieje");
		}
		objects.push_back(next);
	}
	{
		std::lock_guard<std::mutex> lg(multiplierMutex);
		if (next->x > maxX)
			maxX = next->x;
		else if (next->x < minX)
			minX = next->x;
		if (next->y > maxY)
			maxY = next->y;
		else if (next->y < minY)
			minY = next->y;
		if (next->z > maxZ)
			maxZ = next->z;
		else if (next->z < minZ)
			minZ = next->z;
		if (next->d > maxD)
			maxD = next->d;
	}
}

void gravityField::computeGravity(double dt) {
	double multi;
	double rest;
	int intMulti;
	{
		std::lock_guard<std::mutex> lg(multiplierMutex);
		multi = timeMultiplier;
	}
	intMulti = static_cast<int>(multi);
	rest = multi - static_cast<double>(intMulti);
	for (;intMulti>0;--intMulti)
		for (auto i : objects) {
			double Ex = 0.0;
			double Ey = 0.0;
			double Ez = 0.0;
			for (auto j : objects) {
				if (i == j)
					continue;
				else {
					double iDisjSq = i->distanceSquared(*j);
					if (iDisjSq) {
						double MGRRR = G*j->m / (iDisjSq * sqrt(iDisjSq));
						Ex += (j->x - i->x) * MGRRR; //direction j-i
						Ey += (j->y - i->y) * MGRRR;
						Ez += (j->z - i->z) * MGRRR;
					}
				}
			}
			{
				std::lock_guard<std::mutex> lg(objectsMutex);
				i->updateAcceleration(Ex, Ey, Ez);
				i->updatePosition(dt);
				i->updateVelocity(dt);
			}
			{
				std::lock_guard<std::mutex> lg(maxMutex);
				if (i->x > maxX)
					maxX = i->x;
				else if (i->x < minX)
					minX = i->x;
				if (i->y > maxY)
					maxY = i->y;
				else if (i->y < minY)
					minY = i->y;
				if (i->z > maxZ)
					maxZ = i->z;
				else if (i->z < minZ)
					minZ = i->z;
				if (i->d > maxD)
					maxD = i->d;
			}
		}
	if (rest)
		for (auto i : objects) {
			double Ex = 0.0;
			double Ey = 0.0;
			double Ez = 0.0;
			for (auto j : objects) {
				if (i == j)
					continue;
				else {
					double iDisjSq = i->distanceSquared(*j);
					if (iDisjSq) {
						double MGRRR = G*j->m / (iDisjSq * sqrt(iDisjSq));
						Ex += (j->x - i->x) * MGRRR;
						Ey += (j->y - i->y) * MGRRR;
						Ez += (j->z - i->z) * MGRRR;
					}
				}
			}
			{
				std::lock_guard<std::mutex> lg(objectsMutex);
				i->updateAcceleration(Ex, Ey, Ez);
				i->updatePosition(dt*rest);
				i->updateVelocity(dt*rest);
			}
			{
				std::lock_guard<std::mutex> lg(maxMutex);
				if (i->x > maxX)
					maxX = i->x;
				else if (i->x < minX)
					minX = i->x;
				if (i->y > maxY)
					maxY = i->y;
				else if (i->y < minY)
					minY = i->y;
				if (i->z > maxZ)
					maxZ = i->z;
				else if (i->z < minZ)
					minZ = i->z;
				if (i->d > maxD)
					maxD = i->d;
			}
		}
	{
		std::lock_guard<std::mutex> lg(simulTimeMutex);
		simulTime += dt*multi;
	}
}
void gravityField::multiplyMultiplier(double multi) {
	if (multi < 0.0)
		return;
	std::lock_guard<std::mutex> lg(multiplierMutex);
	timeMultiplier *= multi;
	if (timeMultiplier > 10000.0)
		timeMultiplier = 10000.0;
}
void gravityField::printObjects(void) const {
	std::lock_guard<std::mutex> lg(objectsMutex);
	for (const auto x : objects) {
		std::cout << *x;
	}
}
void gravityField::printObjectsList(std::ostream& out, int prec) const {
	std::lock_guard<std::mutex> lg(objectsMutex);
	for (const auto x : objects) {
		out << x->shortDescription(prec);
		out << std::endl;
	}
}
void gravityField::removeObject(const std::string name) {
	std::list<flyingObject*>::const_iterator pos;
	for (pos = objects.cbegin(); pos != objects.cend(); pos++) {
		std::lock_guard<std::mutex> lg(objectsMutex);
		if ((*pos)->name == name) {
			flyingObject* tmp = *pos;
			objects.erase(pos);
			delete tmp;
			break;
		}
	}
	maxX = .0;
	maxY = .0;
	maxZ = .0;
	minX = .0;
	minY = .0;
	minZ = .0;
	maxD = .0;
	{
		std::lock_guard<std::mutex> lg(objectsMutex);
		for (auto i : objects) {
			std::lock_guard<std::mutex> lg(maxMutex);
			if (i->x > maxX)
				maxX = i->x;
			else if (i->x < minX)
				minX = i->x;
			if (i->y > maxY)
				maxY = i->y;
			else if (i->y < minY)
				minY = i->y;
			if (i->z > maxZ)
				maxZ = i->z;
			else if (i->z < minZ)
				minZ = i->z;
			if (i->d > maxD)
				maxD = i->d;
		}
	}
}
bool gravityField::searchObject(const std::string name) const {
	std::lock_guard<std::mutex> lg(objectsMutex);
	for (auto x : objects) {
		if (x->getName() == name)
			return true;
	}
	return false;
}

void gravityField::setTimeMultiplier(double multi) {
	std::lock_guard<std::mutex> lg(multiplierMutex);
	timeMultiplier = multi;
	if (timeMultiplier < 0.0)
		timeMultiplier = 0.0;
}

gravityField::~gravityField(void) {
	std::lock_guard<std::mutex> lg(objectsMutex);
	while (!objects.empty()) {
		delete objects.front();
		objects.pop_front();
	}
}