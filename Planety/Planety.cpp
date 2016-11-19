#include "Planety.h"

//MUTEX
std::mutex fieldMutex;

//CONSTANTS
namespace constants {
	int FRAME = 10;
}
const int& FRAME_SIZE = constants::FRAME;

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
	double currentTime = 0.0;
	double newFrame = frame / static_cast<double>(1000);
	gravField->setTimeMultiplier(1.0);
	while (currentTime < time) {
		gravField->computeGravity(newFrame);
		currentTime += newFrame;
	}
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

void optionsMenu(gravityField* gravField) {
	char choice;
	std::string tmp;
	do {
		std::cout << std::endl;
		std::cout << "0 - Przesun obiekty w czasie" << std::endl;
		std::cout << "1 - Zmien stala ramki czasu" << std::endl;
		std::cout << "2 - zresetuj czas symulacji" << std::endl;
		std::cout << "9 - Wyjdz" << std::endl;
		choice = std::cin.peek();
		std::getline(std::cin, tmp);
	} while (choice < '0' || choice > '2' && choice!='9');
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
	case '9':
		break;
	}
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
