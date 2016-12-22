/*
 * Logger.cpp
 *
 *  Created on: 29 Oct 2016
 *      Author: muttley
 */
#include "Logger.h"
#include <iostream>
#include <ctime>

using namespace std;

int LogLevel = 1;

void LOG(int logLevel, string text)
{
    time_t rawTime;
    tm* timeInfo;
    char buffer[30];

    time(&rawTime);
    timeInfo = localtime(&rawTime);

    strftime(buffer, 30, "[%F %T] ", timeInfo);
    if (logLevel >= LogLevel)
        cout << buffer << text << endl;
}



