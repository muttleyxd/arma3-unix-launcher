/*
 * Logger.cpp
 *
 *  Created on: 29 Oct 2016
 *      Author: muttley
 */
#include "Logger.h"
#include <iostream>

using namespace std;

int LogLevel = 1;

void LOG(int logLevel, string text)
{
	if (logLevel >= LogLevel)
		cout << text << endl;
}



