#pragma once
#include <GL\freeglut.h>
#include <GL\GL.h>
#include "PlanetyClasses.h"
#include <exception>
#include <iostream>
#include <string>
#include <cstdio>
#include <string>
#include <thread>

class threadExit : public std::exception {
private:
	int status;
public:
	int getStatus(void) const { return status; }
	threadExit() :exception{ "exit" }, status{ 0 } {}
	threadExit(const char* msg) :exception{ msg }, status{ 0 } {}
	threadExit(const char* msg, int stat) :exception{ msg }, status{ stat } {}
};

void startRendering(gravityField** gravField,std::mutex* gravMutex);