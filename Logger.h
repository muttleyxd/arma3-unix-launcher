/*
 * Logger.h
 *
 *  Created on: 29 Oct 2016
 *      Author: muttley
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <string>

//0 - full verbose output
//1 - limited output

//1 is default

extern int LogLevel;
void LOG(int logLevel, std::string text);

#endif /* LOGGER_H_ */
