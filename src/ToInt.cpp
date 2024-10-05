#include "main.h"

int ToInt(string str) {
	istringstream iss(str);
	int num;
	iss >> num;

	return num;
}