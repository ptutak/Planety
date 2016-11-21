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
#pragma once

#include <mutex>
#include <list>
#include <cmath>
#include <ctime>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>

// CONSTANTS
constexpr double G = 6.674083131e-11;
constexpr double c = 299792458.0;
constexpr double cSq = 89875517873681764.0;
constexpr double MAX_MULTIPLIER = 100000.0;

//SIMULATION INFO

class simulationInfo {
	int lastFrame;
	int constFrame;
	bool clockOn;
	clock_t startClock;
	clock_t stopClock;
	std::mutex realTimeMutex;
	std::mutex frameMutex;
public:
	void startStopClock();
	void resetClock();

	int getLastFrame(void) { std::lock_guard<std::mutex> lg(frameMutex); return lastFrame; }
	int getConstFrame(void){ std::lock_guard<std::mutex> lg(frameMutex); return constFrame; }
	double getRealTime(void);
	
	void setLastFrame(int fr) {	std::lock_guard<std::mutex> lg(frameMutex);	lastFrame = fr;	}
	void setConstFrame(int fr) { std::lock_guard<std::mutex> lg(frameMutex); constFrame = fr; }

	simulationInfo() : lastFrame{ 0 }, constFrame{ 0 }, clockOn{ false }, startClock{ clock() } { stopClock = startClock; }
};

simulationInfo& getInfo(void);

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
	double recipGamma;

public:
	double distance(const flyingObject& obj) const { return sqrt((obj.x - x)*(obj.x - x) + (obj.y - y)*(obj.y - y) + (obj.z - z)*(obj.z - z)); }
	double distanceSquared(const flyingObject& obj) const { return (obj.x - x)*(obj.x - x) + (obj.y - y)*(obj.y - y) + (obj.z - z)*(obj.z - z); }
	void recalculateGamma(void) {
		recipGamma = sqrt(1.0 - getVSq() / cSq);
	}
	void recalculateVelocity(void) {
		double v = getV();
		if (getVSq() > cSq) {
			vx = vx*c / v;
			vy = vy*c / v;
			vz = vz*c / v;
		}
	}
	virtual void updateAcceleration(double Ex, double Ey, double Ez) {
		ax = Ex * recipGamma;
		ay = Ey * recipGamma;
		az = Ez * recipGamma;
	}
	virtual void updatePosition(double dt) {
		x = x + vx*dt + ax*dt*dt / 2;
		y = y + vy*dt + ay*dt*dt / 2;
		z = z + vz*dt + az*dt*dt / 2;
	}
	virtual void updateVelocity(double dt) {
		vx += dt*ax;
		vy += dt*ay;
		vz += dt*az;
		recalculateGamma();
	}
	virtual std::string shortDescription(int precision = -1) const;

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
	char getType(void) const { return type; }
	double getVSq(void) const { return (vx*vx + vy*vy + vz*vz); }
	double getV(void) const { return sqrt(vx*vx + vy*vy + vz*vz); }
	double getRecipGamma(void) const { return recipGamma; }

	void setName(std::string newName) { name = newName; }
	virtual void setMass(double mass) { m = mass; }
	void setDiameter(double diameter) { d = diameter; }
	void setX(double xX) { x = xX; }
	void setY(double yY) { y = yY; }
	void setZ(double zZ) { z = zZ; }
	void setVx(double Vx) { vx = Vx; }
	void setVy(double Vy) { vy = Vy; }
	void setVz(double Vz) { vx = Vz; }

	flyingObject(std::string oName, double mass = 0.0, double diameter = 0.0, double xX = 0.0, double yY = 0.0, double zZ = 0.0, double vX = 0.0, double vY = 0.0, double vZ = 0.0);
	flyingObject(double mass = 0.0, double diameter = 0.0, double xX = 0.0, double yY = 0.0, double zZ = 0.0, double vX = 0.0, double vY = 0.0, double vZ = 0.0);
	virtual ~flyingObject(void) {}
};

std::ostream& operator<<(std::ostream& out, const flyingObject& obj);

/*

CLASS ROCKET

*/

class rocket :public flyingObject
{
protected:
	double Fxe;
	double Fye;
	double Fze;
	double axe;
	double aye;
	double aze;
	void recalculateEngineAcceleration(void) {
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
public:
	std::string shortDescription(int precision = -1) const;
	void updateAcceleration(double Ex, double Ey, double Ez) {
		ax = (Ex + axe)*recipGamma;
		ay = (Ey + aye)*recipGamma;
		az = (Ez + aze)*recipGamma;
	}

	double getForceX(void) const { return Fxe; }
	double getForceY(void) const { return Fye; }
	double getForceZ(void) const { return Fze; }

	void setMass(double mass) { m = mass;  std::cout << "jest rakieta" << axe; }
	void setForceX(double Fx) { Fxe = Fx; recalculateEngineAcceleration(); }
	void setForceY(double Fy) { Fye = Fy; recalculateEngineAcceleration(); }
	void setForceZ(double Fz) { Fze = Fz; recalculateEngineAcceleration(); }

	rocket(std::string oName, double mass = 0.0, double diameter = 0.0, double xX = 0.0, double yY = 0.0, double zZ = 0.0, double vX = 0.0, double vY = 0.0, double vZ = 0.0, double Fx = 0.0, double Fy = 0.0, double Fz = 0.0);
	rocket(double mass = 0.0, double diameter = 0.0, double xX = 0.0, double yY = 0.0, double zZ = 0.0, double vX = 0.0, double vY = 0.0, double vZ = 0.0, double Fx = 0.0, double Fy = 0.0, double Fz = 0.0);
	rocket(const flyingObject& obj, double Fx = 0.0, double Fy = 0.0, double Fz = 0.0);
	~rocket(void) {}
};

/*

CLASS GRAVITY FIELD

*/

class gravityField
{
	double maxX;
	double minX;
	double maxY;
	double minY;
	double maxZ;
	double minZ;
	double maxD;
	int intMultiplier;
	double restMultiplier;
	double simulTime;
	std::list<flyingObject*> objects;
	mutable std::mutex simulTimeMutex;
	mutable std::mutex multiplierMutex;
	mutable std::mutex maxMutex;
public:
	mutable std::mutex objectsMutex;

	void addMultiplier(double add);
	void addObject(flyingObject* next);
	void computeGravity(double dt);
	void multiplyMultiplier(double multi);
	void printObjects(void) const;
	void printObjectsList(std::ostream& out, int precision = -1) const;
	void removeObject(const std::string name);
	bool searchObject(const std::string name) const;
	
	double getMaxX(void) const { std::lock_guard<std::mutex> lg(maxMutex); return maxX; }
	double getMinX(void) const { std::lock_guard<std::mutex> lg(maxMutex); return minX; }
	double getMaxY(void) const { std::lock_guard<std::mutex> lg(maxMutex); return maxY; }
	double getMinY(void) const { std::lock_guard<std::mutex> lg(maxMutex); return minY; }
	double getMaxZ(void) const { std::lock_guard<std::mutex> lg(maxMutex); return maxZ; }
	double getMinZ(void) const { std::lock_guard<std::mutex> lg(maxMutex); return minZ; }
	double getMaxD(void) const { std::lock_guard<std::mutex> lg(maxMutex); return maxD; }
	double getTimeMultiplier(void) const { std::lock_guard<std::mutex> lg(multiplierMutex); return (static_cast<double>(intMultiplier)+restMultiplier); }
	double getSimulTime(void) const { std::lock_guard<std::mutex> lg(simulTimeMutex); return simulTime; }
	const std::list<flyingObject*>& getObjects(void) const { return objects; }
	
	void resetTotalTime(void) { std::lock_guard<std::mutex> lg(simulTimeMutex); simulTime=0.0; }
	void setTimeMultiplier(double multi);

	gravityField(void) :maxX{ 0.0 }, minX{ 0.0 }, maxY{ 0.0 }, minY{ 0.0 }, maxZ{ 0.0 }, minZ{ 0.0 }, maxD{ 0.0 }, intMultiplier{ 0 }, restMultiplier{ 0.0 }, simulTime{ 0.0 } {};
	~gravityField(void);
};
