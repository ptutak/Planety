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