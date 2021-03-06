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
			std::cerr << exc.what() << std::endl;
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
	if (name == "") {
		std::cout << "OK - nie usuwam" << std::endl;
		return;
	}
	if (gravField->searchObject(name)) {
		gravField->removeObject(name);
		std::cout << "Usunalem: " << name << std::endl;
	}
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

void startSimulation(gravityField* gravField) {
	std::cout << "Ramka czasu: " << FRAME_SIZE << "ms" << std::endl;
	std::cout << "Poczatkowy mnoznik czasu: " << INITIAL_MULTIPLIER << std::endl;
	std::cout << "Nacisnij przycisk, by otworzyc okno symulacji, podczas symulacji nacisnij Escape by przerwac." << std::endl;
	std::cout << "Obsluga:" << std::endl;
	std::cout << "SPACE - ustawia mnoznik czasu na 1.0" << std::endl;
	std::cout << "SHIFT - ustawia mnoznik czasu na 0.0 - zatrzymuje symulacje" << std::endl;
	std::cout << "STRZALKI - przesuwaja obiekty w oknie symulacji wzgledem osi x i y" << std::endl;
	std::cout << "PGUP,PGDOWN - przesuwaja obiekty wzgledem osi z" << std::endl;
	std::cout << "BACKSPACE - resetuje czas rzeczywisty" << std::endl;
	std::cout << "ENTER - start,stop dla czasu rzeczywistego" << std::endl;
	std::cout << "+,- - zwiekszaja/zmniejszaja mnoznik czasu co 1.0" << std::endl;
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

void saveToFile(gravityField* gravField, std::string fileName, int prec) {
	std::ofstream file;
	file.open(fileName, std::fstream::out | std::fstream::trunc);
	if (file.good()) {
		gravField->printObjectsList(file, prec);
		file.close();
	}
	else {
		std::cout << "Bledna nazwa pliku" << std::endl;
	}
}

void saveToFileMenu(gravityField* gravField) {
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
	std::cout << "Podaj precyzje (domyslnie 11)" << std::endl;
	std::string prec;
	int precNumber=11;
	std::getline(std::cin, prec);
	std::stringstream strm(prec);
	strm >> precNumber;
	saveToFile(gravField, filename, precNumber);
}

void readFromFile(gravityField* gravField, std::string fileName) {
	std::lock_guard<std::mutex> lg(fieldMutex);
	std::ifstream file;
	if (fileName == "STDIN")
	{
		while (!std::cin.eof()) {
			flyingObject* tmpObject = readObjectFromStream(std::cin);
			if (tmpObject)
			{
				try {
					gravField->addObject(tmpObject);
				}
				catch (std::invalid_argument x) {
					std::cerr << x.what() << std::endl;
					delete tmpObject;
					tmpObject = nullptr;
				}
			}
//			if (tmpObject)
//				std::cout << "Dodano obiekt: " << tmpObject->shortDescription() << std::endl;
		}
	}
	else {
		file.open(fileName, std::fstream::in);
		if (file.good()) {
			while (!file.eof()) {
				flyingObject* tmpObject = readObjectFromStream(file);
				if (tmpObject)
				{
					try {
						gravField->addObject(tmpObject);
					}
					catch (std::invalid_argument x) {
						std::cerr << x.what() << std::endl;
						delete tmpObject;
						tmpObject = nullptr;
					}
				}
//				if (tmpObject)
//					std::cout << " Dodano obiekt: " << tmpObject->shortDescription() << std::endl;
			}
			file.close();
		}
		else {
			std::cout << "Niepoprawna nazwa pliku" << std::endl;
		}
	}
}

void readFromFileMenu(gravityField* gravField) {
	std::string filename;
	std::cout << std::endl;
	std::cout << "Podaj nazwe pliku" << std::endl;
	std::getline(std::cin, filename);
	readFromFile(gravField, filename);
	std::cout << "Obiekty w symulacji:" << std::endl;
	gravField->printObjectsList(std::cout);
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
void computeTime(gravityField* gravField, double time,double newFrame) {
	long long currentTime = 0;
	long long intTime = 0;
	double restTime = 0.0;
	gravField->setTimeMultiplier(MAX_MULTIPLIER);
	intTime = static_cast<long long>((time / newFrame) / MAX_MULTIPLIER);
	restTime = time - static_cast<double>(intTime)*MAX_MULTIPLIER*newFrame;
	while (currentTime < intTime) {
		gravField->computeGravity(newFrame);
		currentTime++;
	}
	gravField->setTimeMultiplier(restTime / newFrame);
	gravField->computeGravity(newFrame);
}

void computeTimeMenu(gravityField* gravField) {
	std::cout << "Podaj czas obliczen w s:" << std::endl;
	double time;
	std::cin >> time;
	std::cout << "Podaj wielkosc ramki w ms:" << std::endl;
	int frame;
	std::cin >> frame;
	computeTime(gravField, time, frame / static_cast<double>(1000));
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
		} while (newFrame <= 0);
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

INIT PLANETY

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
			startSimulation(gravField);
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
			{
				std::lock_guard<std::mutex> lg(fieldMutex);
				delete gravField;
				std::cout << "Goodbye" << std::endl;
				return;
			}
		}
	} while (1);
}

void initPlanety(int argc, char* argv[]) {
	gravityField* gravField = new gravityField;
	std::string output = "";
	std::string input = "";
	int prec=-1;
	double time=0.0;
	bool compute = false;
	bool owrite = false;
	bool iread = false;
	for (int i = 1; i < argc; ++i) {
		if (std::string(argv[i]) == "-i") {
			iread = true;
			if (i + 1<argc) {
				input = std::string(argv[i + 1]);
				i++;
			}
		}
		else if (std::string(argv[i]) == "-f") {
			if (i + 1<argc) {
				std::stringstream tmp;
				tmp << argv[i + 1];
				int frame;
				tmp >> frame;
				if (frame > 0) {
					const_cast<int&>(FRAME_SIZE) = frame;
				}
				i++;
			}
		}
		else if (std::string(argv[i]) == "-m") {
			if (i + 1<argc) {
				std::stringstream tmp;
				tmp << argv[i + 1];
				double multi;
				tmp >> multi;
				if (multi>=0.0) {
					const_cast<double&>(INITIAL_MULTIPLIER) = multi;
				}
				i++;
			}
		}
		else if (std::string(argv[i]) == "-o") {
			owrite = true;
			if (i + 1<argc) {
				output == std::string(argv[i + 1]);
				i++;
			}
		}
		else if (std::string(argv[i]) == "-p") {
			if (i + 1<argc) {
				std::stringstream tmp;
				tmp << argv[i + 1];
				tmp >> prec;
			}
		}
		else if (std::string(argv[i]) == "-c") {
			if (i + 1<argc) {
				std::stringstream tmp;
				tmp << argv[i + 1];
				tmp >> time;
				compute = true;
			}
		}
		else if ((std::string(argv[i]) == "-h") || (std::string(argv[i]) == "--help")) {
			std::cout << "Uzycie linii polecen:" << std::endl;
			std::cout << "-h, --help	: pomoc" << std::endl << std::endl;
			std::cout << "-i plik		: wczytuje plik wejsciowy dla symulacji - domyslnie STDIN" << std::endl << std::endl;
			std::cout << "-o plik		: ustawia plik wyjsciowy do ktorego bedzie zapisany stan po zakonczeniu symulacji - domyslnie STDOUT" << std::endl<<std::endl;
			std::cout << "-p precyzja	: jesli byl podany parametr -o, ustawia precyzje zapisu danych do pliku, domyslnie - do 4 miejsc" << std::endl<<std::endl;
			std::cout << "-f rozmiar	: ustawia rozmiar ramki czasu w ms (liczba calkowita), np. 20 dla 20ms, domyslnie: "<<FRAME_SIZE << std::endl<<std::endl;
			std::cout << "-m mnoznik	: ustawia mnoznik czasu z ktorym rozpocznie sie symulacja (liczba zmiennoprzecinkowa), np. 2.0" << std::endl;
			std::cout << "-c czas		: wykonuje obliczenia symulacyjne dla podanego czasu zakonczenia w sekundach (liczba zmiennoprzecinkowa)" << std::endl;
			return;
		}
	}
	if (iread)
		if (input!="")
			readFromFile(gravField, input);
		else
			readFromFile(gravField, "STDIN");

	if (compute)
		computeTime(gravField, time, FRAME_SIZE / static_cast<double>(CLOCKS_PER_SEC));
	else
		startSimulation(gravField);
	
	if (owrite)
		if (output != "" && output!="STDOUT")
			saveToFile(gravField, output, prec);
		else
			gravField->printObjectsList(std::cout, prec);
	
}