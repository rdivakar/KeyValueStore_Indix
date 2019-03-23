#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <time.h>
#include <stdarg.h>
#include <sstream>
using namespace std;

#pragma warning(disable:4251)

#ifdef KEY_VALUE_STORE_EXPORTS
#		define LOGGER_API __declspec(dllexport)
#	else
#		define LOGGER_API  __declspec(dllimport)
#endif

class LOGGER_API Logger
{
	static Logger *logInstance;
	ofstream outfile;
	Logger();
	~Logger();
	static string GetTime();
public:
	static Logger* GetLoggerInstance();
	
	void Log(const char* message,...);
};

