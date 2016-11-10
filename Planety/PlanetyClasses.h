#pragma once

#include <mutex>
#include <list>
#include <cmath>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>


//MUTEX LOCKS

std::mutex& getReadWriteMutex(void);
std::mutex& getMultiplierMutex(void);

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
	virtual std::string shortDescription(int precision = -1);

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


	flyingObject(std::string oName, double mass = 0.0, double diameter = 0.0, double xX = 0.0, double yY = 0.0, double zZ = 0.0, double vX = 0.0, double vY = 0.0, double vZ = 0.0);
	flyingObject(double mass = 0.0, double diameter = 0.0, double xX = 0.0, double yY = 0.0, double zZ = 0.0, double vX = 0.0, double vY = 0.0, double vZ = 0.0);
	virtual ~flyingObject(void) {}
};

std::ostream& operator<<(std::ostream& out, const flyingObject obj);

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
	std::string shortDescription(int precision = -1);
	void updateAcceleration(double Ex, double Ey, double Ez);

	double getForceX(void) const { return Fxe; }
	double getForceY(void) const { return Fye; }
	double getForceZ(void) const { return Fze; }

	void setMass(double mass) { m = mass; recalculateEngineAcceleration(); }
	void setForceX(double Fx) { Fxe = Fx; }
	void setForceY(double Fy) { Fye = Fy; }
	void setForceZ(double Fz) { Fze = Fz; }

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

	double multiplier;

	std::list<flyingObject*> objects;

public:
	void addObject(flyingObject* next);
	void computeGravity(double dt);
	void printObjects(void) const;
	void printObjectsList(std::ostream& out, int precision = -1) const;
	void removeObject(const std::string name);
	bool searchObject(const std::string name) const;

	double getMaxX(void) const { return maxX; }
	double getMinX(void) const { return minX; }
	double getMaxY(void) const { return maxY; }
	double getMinY(void) const { return minY; }
	double getMaxZ(void) const { return maxZ; }
	double getMinZ(void) const { return minZ; }
	double getMaxD(void) const { return maxD; }
	double getMultiplier(void) const { return multiplier; }
	const std::list<flyingObject*>& getObjects(void) const { return objects; }
	
	void addMultiplier(double add) { multiplier += add; }

	gravityField(void) :maxX{ 0.0 }, minX{ 0.0 }, maxY{ 0.0 }, minY{ 0.0 }, maxZ{ 0.0 }, minZ{ 0.0 }, maxD{ 0.0 }, multiplier{ 1.0 } {};
	~gravityField(void);
};
