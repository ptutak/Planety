#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <Windows.h>
#include "PlanetyClasses.h"
#include "Planety.h"

//CONSTANTS
const int FRAME_SIZE = 10;

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
	clock_t dif = FRAME_SIZE;
	double frSiz = static_cast<double>(FRAME_SIZE) / static_cast<double>(CLOCKS_PER_SEC);
	int multiplier,initialMultiplier=1;
	system("cls");
	std::cout << "Podaj szybkosc symulacji w krotnosci czasu (np. 1,2,3...) lub nacisnij Enter (domyslnie 1)" << std::endl;
	{
		std::string tmp;
		std::getline(std::cin, tmp);
		std::istringstream tmpstr(tmp);
		tmpstr >> initialMultiplier;
	}
	multiplier = initialMultiplier;
	std::cout << "Mnoznik czasu:" << initialMultiplier << std::endl;
	std::cout << "Nacisnij Enter, by rozpoczac symulacje, podczas symulacji nacisnij Escape by przerwac." << std::endl;
	system("pause");
	start_clock = clock();
	while (!(GetAsyncKeyState(VK_ESCAPE))) {
		do {
			gravField->computeGravity(frSiz);
			--multiplier;
		} while (multiplier);
		multiplier = initialMultiplier;
		do
			dif = clock() - start_clock;
		while (dif < FRAME_SIZE);
		start_clock = clock();					//pocz¹tek liczenia czasu kolejnej iteracji
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

void init(void) {
	
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
			return;
		}
	} while (1);
}
