#include "Planety.h"
#include <iostream>

int main(void)
{

	//	init();
	gravityField* tmp = new gravityField;
	tmp->addObject(new flyingObject{ "Ziemia",1000,100000,0,0,15 });
	tmp->addObject(new flyingObject{ "Wenus",10000, 1000,150000 });
	try {
		startRendering(&tmp);
	}
	catch (const threadExit& x) {
		std::cout << x.what()<<std::endl;
	}
	catch (const std::exception& x) {
		std::cout << x.what() << std::endl;
	}
	catch (...) {
		std::cout << "nieoczekiwany wyjatek" << std::endl;
		system("pause");
		return 0;
	}
//	std::cout << "jestem tu, po wyjsciu z loop" << std::endl;
//	system("pause");
}
	
