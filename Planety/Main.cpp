#include "Planety.h"
#include <iostream>

int main(void)
{
/*	std::mutex mutex;
	gravityField* tmp = new gravityField;
	tmp->addObject(new flyingObject{ "Ziemia",5.9722e+24,0,0,0,0,0,0,0 });
	tmp->addObject(new flyingObject{ "Object1",1,0,6378000,0,0,0,7910,0 });
	try {
		startRendering(&tmp,&mutex);
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
*/	
	//	std::cout << "jestem tu, po wyjsciu z loop" << std::endl;

	init();
	system("pause");
}
	
