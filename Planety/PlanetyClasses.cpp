#include "PlanetyClasses.h"


//FRAME INFO
simulationInfo info;

simulationInfo& getInfo(void) { return info; }

double simulationInfo::getRealTime(void) {
	std::lock_guard<std::mutex> lg(realTimeMutex);
	if (clockOn)
		return ((clock() - startClock) / static_cast<double>(CLOCKS_PER_SEC));
	else
		return ((stopClock - startClock) / static_cast<double>(CLOCKS_PER_SEC));
}
void simulationInfo::startStopClock() {
	std::lock_guard<std::mutex> lg(realTimeMutex);
	if (clockOn)
	{
		stopClock = clock();
		clockOn = false;
	}
	else {
		startClock += clock()-stopClock;
		clockOn = true;
	}
}
void simulationInfo::resetClock(void) {
	std::lock_guard<std::mutex> lg(realTimeMutex);
	startClock = clock();
	stopClock = startClock;
}

/*

CLASS FLYING OBJECT

*/

int flyingObject::oNumber = 0;

std::ostream& operator<<(std::ostream& out, const flyingObject& obj) {
	out << obj.getName() << std::endl;
	out << "m:  " << obj.getMass() << " kg"<<std::endl;
	out << "d:  " << obj.getDiameter() << " m"<<std::endl;
	out << "x:  " << obj.getX() << " m"<<std::endl;
	out << "y:  " << obj.getY() << " m" << std::endl;
	out << "z:  " << obj.getZ() << " m" << std::endl;
	out << "Vx: " << obj.getVx() << " m/s" << std::endl;
	out << "Vy: " << obj.getVy() << " m/s" << std::endl;
	out << "Vz: " << obj.getVz() << " m/s" << std::endl;
	out << "ax: " << obj.getAx() << " m/s^2" << std::endl;
	out << "ay: " << obj.getAy() << " m/s^2" << std::endl;
	out << "az: " << obj.getAz() << " m/s^2" << std::endl;
	if (obj.getType() == 'r') {
		const rocket robj = dynamic_cast<const rocket&>(obj);
		out << "Fx: " << robj.getForceX() << " N" << std::endl;
		out << "Fy: " << robj.getForceY() << " N" << std::endl;
		out << "Fz: " << robj.getForceZ() << " N" << std::endl;
	}
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
flyingObject::flyingObject(std::string oName, double mass, double diameter, double xX, double yY, double zZ, double vX, double vY, double vZ) :
	name{ oName }, type{ 'o' }, m{ mass }, d{ diameter }, x{ xX }, y{ yY }, z{ zZ }, vx{ vX }, vy{ vY }, vz{ vZ } {
	oNumber++;
	ax = 0.0;
	ay = 0.0;
	az = 0.0;
	if (getVSq() > cSq) {
		vx = vx*c / getV();
		vy = vy*c / getV();
		vz = vz*c / getV();
	}
	recalculateGamma();
}
flyingObject::flyingObject(double mass, double diameter, double xX, double yY, double zZ, double vX, double vY, double vZ) :
	m{ mass }, type{ 'o' }, d{ diameter }, x{ xX }, y{ yY }, z{ zZ }, vx{ vX }, vy{ vY }, vz{ vZ } {
	name = "Object" + std::to_string(oNumber);
	oNumber++;
	ax = 0.0;
	ay = 0.0;
	az = 0.0;
	if (getVSq() > cSq) {
		vx = vx*c / getV();
		vy = vy*c / getV();
		vz = vz*c / getV();
	}
	recalculateGamma();
}

/*

CLASS ROCKET

*/

std::string rocket::shortDescription(int prec) const {
	std::string retString, tempString;
	std::stringstream strm;
	retString = flyingObject::shortDescription(prec);
	if (prec >= 0)
		strm << std::setprecision(prec);
	strm.str(""); strm.clear(); strm << Fxe; tempString = strm.str(); retString += ',' + tempString;
	strm.str(""); strm.clear(); strm << Fye; tempString = strm.str(); retString += ',' + tempString;
	strm.str(""); strm.clear(); strm << Fze; tempString = strm.str(); retString += ',' + tempString;
	return retString;
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
	double multi = static_cast<double>(intMultiplier) + restMultiplier;
	multi += add;
	if (multi < 0.0)
		multi = 0.0;
	if (multi > 10000.0)
		multi = 10000.0;
	intMultiplier = static_cast<int>(multi);
	restMultiplier = multi - static_cast<double>(intMultiplier);
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
	int intMulti;
	double restMulti;
	{
		std::lock_guard<std::mutex> lg(multiplierMutex);
		intMulti = intMultiplier;
		restMulti = restMultiplier;
	}
	double totalMulti = static_cast<double>(intMulti) + restMulti;
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
	if (restMulti)
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
				i->updatePosition(dt*restMulti);
				i->updateVelocity(dt*restMulti);
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
		simulTime += dt*totalMulti;
	}
}
void gravityField::multiplyMultiplier(double multi) {
	if (multi < 0.0)
		return;
	std::lock_guard<std::mutex> lg(multiplierMutex);
	multi *= (static_cast<double>(intMultiplier) + restMultiplier);
	if (multi > 10000.0) {
		intMultiplier = 10000;
		restMultiplier = 0.0;
	}
	else {
		intMultiplier = static_cast<int>(multi);
		restMultiplier = multi - static_cast<double>(intMultiplier);
	}
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
	if (multi > 10000.0)
		multi = 10000.0;
	else if (multi < 0.0)
		multi = 0.0;
	intMultiplier = static_cast<int>(multi);
	restMultiplier = multi - static_cast<double>(intMultiplier);
}

gravityField::~gravityField(void) {
	std::lock_guard<std::mutex> lg(objectsMutex);
	while (!objects.empty()) {
		delete objects.front();
		objects.pop_front();
	}
}