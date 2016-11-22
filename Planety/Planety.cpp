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
#include "Planety.h"

//MUTEX
std::mutex fieldMutex;

//CONSTANTS
namespace constants {
	int FRAME_SIZE = 10;
	double INITIAL_MULTIPLIER = 0.0;
}
const int& FRAME_SIZE = constants::FRAME_SIZE;
const double& INITIAL_MULTIPLIER = constants::INITIAL_MULTIPLIER;
/*

INPUT/OUTPUT FUNCTIONS

*/

flyingObject* readObjectFromStream(std::istream& in) {
	flyingObject* tmpObject = nullptr;
	std::string line;
	std::getline(in, line);
	if (line != "" && line[0]!='#') {
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
				if (data.get()=='c')
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
	}
	return tmpObject;
}

/*

MENU - FUNCTIONS

*/

void addObjectMenu(gravityField* gravField) {
	std::cout << std::endl;
	std::cout << "Podaj dane obiektu w formacie:" << std::endl;
	std::cout << "[Nazwa][,Masa[,Srednica[,x[,y[,z[,Vx[,Vy[,Vz[,Fx[,Fy[,Fz]]...]" << std::endl;
	std::cout << "np.: Ziemia,5.9722e+24,6000000,0,0,0,0,0,0" << std::endl;
	flyingObject* made = readObjectFromStream(std::cin);
	if (made) {
		try {
			gravField->addObject(made);
		}
		catch (const std::invalid_argument& exc) {
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
		std::cout << "Podaj nowe dane dla obiektu " + name + " w formacie: [Nazwa][,Masa[,Srednica[,x[,y[,z[,Vx[,Vy[,Vz[,Fx[,Fy[,Fz]]...]" << std::endl;
		std::cout << "np.: Ziemia, 5.9722e+24, 6000000, 0, 0, 0, 0, 0, 0" << std::endl;
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
	else {
		std::cout << "Bledna nazwa obiektu" << std::endl;
	}
}

void startSimulationMenu(gravityField* gravField) {
	system("cls");
	std::cout << "Nacisnij przycisk, by otworzyc okno symulacji, podczas symulacji nacisnij Escape by przerwac." << std::endl;
	std::cout << "Obsluga:" << std::endl;
	std::cout << "SPACE - ustawia mnoznik czasu na 1.0" << std::endl;
	std::cout << "SHIFT - ustawia mnoznik czasu na 0.0 - zatrzymuje symulacje" << std::endl;
	std::cout << "STRZALKI - przesuwaja obiekty w oknie symulacji wzgledem osi x i y" << std::endl;
	std::cout << "PGUP,PGDOWN - przesuwaja obiekty wzgledem osi z" << std::endl;
	std::cout << "BACKSPACE - resetuje czas rzeczywisty" << std::endl;
	std::cout << "ENTER - start,stop dla czasu rzeczywistego" << std::endl;
	std::cout << "+,- - zwiekszaja/zmniejszaja mnoznik czasu co 0.5" << std::endl;
	std::cout << "*,/ - mnoza mnoznik czasu przez 2, 0.5" << std::endl;
	std::cout << "KOLKO MYSZKI - zwieksza, zmniejsza obiekty w oknie symulacji" << std::endl;
	std::cout << "NACISNIJ I PRZYTRZYMAJ LEWY PRZYCISK MYSZY - by zaczac obracac obiektami" << std::endl;
	std::cout << "PRAWY PRZYCISK MYSZY - menu podreczne" << std::endl;
	system("pause");
	clock_t start_clock;
	clock_t dif = FRAME_SIZE;
	getInfo().setConstFrame(FRAME_SIZE);
	gravField->setTimeMultiplier(INITIAL_MULTIPLIER);
	double frSiz = static_cast<double>(FRAME_SIZE) / static_cast<double>(CLOCKS_PER_SEC);
	std::thread renderingThread;
	try {
		renderingThread = std::thread(startRendering, &gravField, &fieldMutex);
	}
	catch (const threadExit& x) {
		std::cerr << "Nastapil wyjatek:" << x.what() << std::endl;
		throw;
	}
	catch (const std::exception& x) {
		std::cerr << x.what() << std::endl;
	}
	start_clock = clock();
	getInfo().startStopClock();
	while (!(GetAsyncKeyState(VK_ESCAPE))) {
		getInfo().setLastFrame(dif);
		gravField->computeGravity(frSiz);
		int rest = 0;
		if (dif > FRAME_SIZE) {
			int multi = dif - FRAME_SIZE;
			rest = multi%FRAME_SIZE;
			multi = multi / FRAME_SIZE;
			for (; multi > 0; --multi)
				gravField->computeGravity(frSiz);
		}
		do {
			dif = clock() - start_clock;
			dif += rest;
		} while (dif < FRAME_SIZE);
		start_clock = clock();
	}
	renderingThread.join();
}

void saveToFileMenu(const gravityField* gravField) {
	std::string filename="";
	std::cout << std::endl;
	std::cout << "Podaj nazwe pliku" << std::endl;
	std::getline(std::cin, filename);
	std::ifstream check(filename, std::ifstream::in);
	if (check.good()) {
		check.close();
		std::cout << "Plik \"" << filename << "\" juz istnieje, czy nadpisac? [tT/cokolwiek]" << std::endl;
		char choice;
		choice = std::cin.get();
		std::string tmp;
		std::getline(std::cin, tmp);
		if ((choice != 't') && (choice != 'T')) {
			std::cout << "Ok - nie zapisuje";
			return;
		}
	}
	std::ofstream file;
	file.open(filename, std::fstream::out | std::fstream::trunc);
	if (file.good()) {
		std::cout << "Podaj precyzje (domyslnie 11)" << std::endl;
		std::string prec;
		int precNumber = 11;
		std::getline(std::cin, prec);
		std::stringstream strm(prec);
		strm >> precNumber;
		gravField->printObjectsList(file,precNumber);
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
	else {
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
			std::lock_guard<std::mutex> lg(fieldMutex);
			delete gravField;
			gravField = new gravityField;
			std::cout << "OK - usunieto" << std::endl;
			return;
	}
	std::cout << "Nie usuwam" << std::endl;
}

void computeTimeMenu(gravityField* gravField) {
	std::cout << "Podaj czas obliczen w s:" << std::endl;
	double time;
	std::cin >> time;
	std::cout << "Podaj wielkosc ramki w ms:" << std::endl;
	int frame;
	std::cin >> frame;
	long long currentTime = 0;
	double newFrame = frame / static_cast<double>(1000);
	long long intTime = 0;
	double restTime = 0.0;
	gravField->setTimeMultiplier(MAX_MULTIPLIER);
	intTime = static_cast<long long>((time/newFrame) / MAX_MULTIPLIER);
	restTime = time - static_cast<double>(intTime)*MAX_MULTIPLIER*newFrame;
	while (currentTime < intTime) {
		gravField->computeGravity(newFrame);
		currentTime++;
	}
	gravField->setTimeMultiplier(restTime/newFrame);
	gravField->computeGravity(newFrame);
	gravField->printObjects();
	std::string tmp;
	std::getline(std::cin, tmp);
}

void changeConstTimeMenu(void) {
	std::cout << "UWAGA!!!" << std::endl;
	std::cout << "Zmiana stalej ramki czasu moze wplynac na dokladnosc i czas obliczen," << std::endl;
	std::cout << "dokonujac tych zmian robisz to na wlasna odpowiedzialnosc!"<<std::endl;
	std::cout << "Czy na pewno chcesz zmienic stala ramki czasu? [Tt/cokolwiek]" << std::endl;
	char c;
	std::string tmp;
	std::cin >> c;
	std::getline(std::cin, tmp);
	if (c == 'T' || c == 't') {
		std::cout << "Obecna ramka czasu: " << FRAME_SIZE <<"ms"<< std::endl;
		std::cout << "Podaj nowa ramke czasu w milisekundach - liczba calkowita dodatnia" << std::endl;
		int newFrame;
		do {
			newFrame;
			std::cin >> newFrame;
		} while (newFrame < 0);
		std::getline(std::cin, tmp);
		const_cast<int&>(FRAME_SIZE) = newFrame;
		std::cout << "Zmieniono, nowa ramka to: " << FRAME_SIZE << "ms" << std::endl;
		return;
	}
	std::cout << "OK - nie zmieniam" << std::endl;
}

void resetSimulTimeMenu(gravityField* gravField) {
	std::cout << "Czy na pewno chcesz zresetowac czas symulacji?[Tt/cokolwiek]" << std::endl;
	char c;
	std::string tmp;
	std::cin >> c;
	std::getline(std::cin, tmp);
	if (c == 'T' || c == 't') {
		gravField->resetTotalTime();
		std::cout << "OK - zresetowano" << std::endl;
		return;
	}
	std::cout << "Nie resetuje" << std::endl;
}

void setInitialMultiplier(void) {
	std::cout << "Podaj poczatkowy mnoznik czasu (aktualnie " <<INITIAL_MULTIPLIER<<")"<< std::endl;
	std::string tmp;
	std::getline(std::cin, tmp);
	double multi=0.0;
	std::stringstream strm(tmp);
	strm >> multi;
	if (multi >0.0)
		const_cast<double&>(INITIAL_MULTIPLIER) = multi;
	std::cout << "OK - nowy poczatkowy mnoznik to: " << INITIAL_MULTIPLIER << std::endl;
}

void optionsMenu(gravityField* gravField) {
	char choice;
	std::string tmp;
	do {
		std::cout << std::endl;
		std::cout << "0 - Przesun obiekty w czasie" << std::endl;
		std::cout << "1 - Zmien stala ramki czasu" << std::endl;
		std::cout << "2 - Zresetuj czas symulacji" << std::endl;
		std::cout << "3 - Ustaw mnoznik poczatkowy" << std::endl;
		std::cout << "9 - Wyjdz" << std::endl;
		choice = std::cin.peek();
		std::getline(std::cin, tmp);
	} while (choice < '0' || choice > '3' && choice!='9');
	switch (choice) {
	case '0':
		computeTimeMenu(gravField);
		break;
	case '1':
		changeConstTimeMenu();
		break;
	case '2':
		resetSimulTimeMenu(gravField);
		break;
	case '3':
		setInitialMultiplier();
		break;
	case '9':
		break;
	}
}

/*

INIT

*/

void initPlanety(void) {
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
			std::cout << "7 - Usun wszystkie obiekty" << std::endl;
			std::cout << "8 - Opcje" << std::endl;
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
			optionsMenu(gravField);
			break;
		case '9':
			std::lock_guard<std::mutex> lg(fieldMutex);
			delete gravField;
			std::cout << "Goodbye" << std::endl;
			return;
		}
	} while (1);
}
