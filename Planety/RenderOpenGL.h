#pragma once
#include <GL\freeglut.h>
#include <GL\GL.h>
#include "PlanetyClasses.h"
#include <exception>
#include <iostream>
#include <string>

class threadExit : public std::exception {
public:
	threadExit(const char* msg) :exception{ msg } {	}
};

void startRendering(gravityField** gravField);