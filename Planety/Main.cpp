#include "Planety.h"
#include <iostream>


int main(void)
{
//	init();
	gravityField* tmp = new gravityField;
		//=new gravityField;
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
	
	system("pause");
}