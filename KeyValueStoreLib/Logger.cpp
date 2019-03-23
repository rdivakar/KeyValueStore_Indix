
#include "Common.h"
#include "Logger.h"
#include <windows.h>


/*
	A Simple logger class to log the information to file for troubleshooting purpose. 
	No file roll over logic considered here, to keep it simple.
*/


Logger* Logger::logInstance = NULL;

Logger::Logger()
{
	stringstream logFileName;
	logFileName << __MODULE_NAME_ << ".log";
	outfile.open(logFileName.str().c_str(), ios::app);
}


Logger::~Logger()
{
	outfile.close();
	if (logInstance)
		delete logInstance;
}

string Logger::GetTime()
{
	time_t t = time(NULL);
	struct tm buf;
	char str[64] = "";
	localtime_s(&buf, &t);
	strftime(str, sizeof(str), "%Y-%m-%d %X", &buf);
	return string(str);
}

Logger * Logger::GetLoggerInstance()
{
	if (logInstance == NULL)
	{
		logInstance = new Logger();
	}
	return logInstance;
}

void Logger::Log(const char* message, ... )
{
	va_list args;
	va_start(args, message);
	
	char buffer[__LOG_BUFF_SIZE__+1] = "";
	stringstream logMessage("");

	vsprintf_s(buffer, message, args);

	logMessage << GetTime() << "	" << buffer << endl;

	outfile << logMessage.str() << endl;
}
