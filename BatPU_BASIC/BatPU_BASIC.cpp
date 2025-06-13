#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include "parser.h"

int main()
{
	try {
		node root = parse("test_basic");
		std::cout << root.str();
	}
	catch (std::exception& e) {
		std::cout << "ERROR : " << e.what() << '\n';
	}
	
}
