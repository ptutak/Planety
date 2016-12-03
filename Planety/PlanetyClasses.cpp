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
#include "PlanetyClasses.h"

// MISCELLANOUS

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
		tmp << years << "y ";
	if (days)
		tmp << days << "d ";
	if (hours)
		tmp << hours << "h ";
	if (mins)
		tmp << mins << "m ";
	tmp << restSec << "s";
	return tmp.str();
}

/*

SIMULATION INFO

*/

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
	out << "time: " << reformatSec(obj.getTime()) << std::endl;
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
	retString += ",0,0,0";
	strm << std::setprecision(3);
	strm.str(""); strm.clear(); strm << col.r; tempString = strm.str(); retString += ',' + tempString;
	strm.str(""); strm.clear(); strm << col.g; tempString = strm.str(); retString += ' ' + tempString;
	strm.str(""); strm.clear(); strm << col.b; tempString = strm.str(); retString += ' ' + tempString;
	return retString;
}
flyingObject::flyingObject(std::string oName, double mass, double diameter, double xX, double yY, double zZ, double vX, double vY, double vZ) :
	name{ oName }, type{ 'o' }, m{ mass }, d{ diameter }, x{ xX }, y{ yY }, z{ zZ }, vx{ vX }, vy{ vY }, vz{ vZ }, time{ 0.0 } ,col{ 1.0,1.0,0.94 } {
	oNumber++;
	if (d <= 0.0) 
		d = 0.0;
	ax = 0.0;
	ay = 0.0;
	az = 0.0;
	recalculateVelocity();
	recalculateGamma();
}
flyingObject::flyingObject(double mass, double diameter, double xX, double yY, double zZ, double vX, double vY, double vZ) :
	m{ mass }, type{ 'o' }, d{ diameter }, x{ xX }, y{ yY }, z{ zZ }, vx{ vX }, vy{ vY }, vz{ vZ }, time{ 0.0 },col{ 1.0,1.0,0.94 } {
	name = "Object" + std::to_string(oNumber);
	oNumber++;
	if (d <= 0.0)
		d = 0.0;
	ax = 0.0;
	ay = 0.0;
	az = 0.0;
	recalculateVelocity();
	recalculateGamma();
}


/*

CLASS ROCKET

*/

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
	strm << std::setprecision(3);
	strm.str(""); strm.clear(); strm << col.r; tempString = strm.str(); retString += ',' + tempString;
	strm.str(""); strm.clear(); strm << col.g; tempString = strm.str(); retString += ' ' + tempString;
	strm.str(""); strm.clear(); strm << col.b; tempString = strm.str(); retString += ' ' + tempString;
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

INPUT/OUTPUT FUNCTIONS

*/

flyingObject* readObjectFromStream(std::istream& in) {
	flyingObject* tmpObject = nullptr;
	std::string line;
	std::getline(in, line);
	if (line != "" && line[0] != '#' && (line[0]>32)) {
		tmpObject = new flyingObject;
		std::stringstream linestrm(line);
		line = "";
		std::getline(linestrm, line, ',');
		if (line != "")
			tmpObject->setName(line);
		if (!linestrm.eof()) {
			std::string line;
			std::getline(linestrm, line, ',');
			double number = 0.0;
			std::stringstream data(line);
			data >> number;
			if (number != 0.0)
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
				if (data.get() == 'c')
					tmpObject->setVx(number*c);
				else
					tmpObject->setVx(number);
		}
		if (!linestrm.eof()) {
			std::string line;
			std::getline(linestrm, line, ',');
			double number = 0.0;
			std::stringstream data(line);
			data >> number;
			if (number != 0.0)
				if (data.get() == 'c')
					tmpObject->setVy(number*c);
				else
					tmpObject->setVy(number);
		}
		if (!linestrm.eof()) {
			std::string line;
			std::getline(linestrm, line, ',');
			double number = 0.0;
			std::stringstream data(line);
			data >> number;
			if (number != 0.0)
				if (data.get() == 'c')
					tmpObject->setVz(number*c);
				else
					tmpObject->setVz(number);
		}
		tmpObject->recalculateVelocity();
		tmpObject->recalculateGamma();
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
				std::stringstream data(line);
				data >> number;
				if (number != 0.0)
					tmpRocket->setForceZ(number);
			}
			if (tmpRocket->getForceX()*tmpRocket->getForceX() + tmpRocket->getForceY()*tmpRocket->getForceY() + tmpRocket->getForceZ()*tmpRocket->getForceZ()) {
				delete tmpObject;
				tmpObject = dynamic_cast<flyingObject*>(tmpRocket);
			}
			else
				delete tmpRocket;
		}
		if (!linestrm.eof()) {
			std::string line;
			std::getline(linestrm, line);
			color col = { 1.0,1.0,0.94};
			std::stringstream data(line);
			data >> col.r >> col.g >> col.b;
			tmpObject->setColor(col);
		}
	}
	return tmpObject;
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
	if (multi > MAX_MULTIPLIER)
		multi = MAX_MULTIPLIER;
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
		objectsSafe.push_back(next);
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
	if (multi > MAX_MULTIPLIER) {
		intMultiplier = static_cast<int>(MAX_MULTIPLIER);
		restMultiplier = 0.0;
	}
	else {
		intMultiplier = static_cast<int>(multi);
		restMultiplier = multi - static_cast<double>(intMultiplier);
	}
}
void gravityField::printObjects(void) const {
	std::lock_guard<std::mutex> lg(objectsMutex);
	for (auto x : objectsSafe) {
		std::cout << *x;
	}
}
void gravityField::printObjectsList(std::ostream& out, int prec) const {
	std::lock_guard<std::mutex> lg(objectsMutex);
	for (auto x : objectsSafe) {
		out << x->shortDescription(prec);
		out << std::endl;
	}
}
void gravityField::removeObject(const std::string name) {
	std::list<flyingObject*>::iterator pos;
	std::list<const flyingObject*>::iterator posS;
	for (pos = objects.begin(),posS=objectsSafe.begin(); pos != objects.end(); ++pos,++posS) {
		std::lock_guard<std::mutex> lg(objectsMutex);
		if ((*pos)->name == name) {
			flyingObject* tmp = *pos;
			objects.erase(pos);
			objectsSafe.erase(posS);
			delete tmp;
			break;
		}
	}
	maxX = 0.0;
	maxY = 0.0;
	maxZ = 0.0;
	minX = 0.0;
	minY = 0.0;
	minZ = 0.0;
	maxD = 0.0;
	{
		std::lock_guard<std::mutex> lg(objectsMutex);
		for (auto i : objectsSafe) {
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
	for (auto x : objectsSafe) {
		if (x->getName() == name)
			return true;
	}
	return false;
}

void gravityField::setTimeMultiplier(double multi) {
	std::lock_guard<std::mutex> lg(multiplierMutex);
	if (multi > MAX_MULTIPLIER)
		multi = MAX_MULTIPLIER;
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
		objectsSafe.pop_front();
	}
}