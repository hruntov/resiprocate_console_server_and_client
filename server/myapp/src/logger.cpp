#include "logger.h"

#include <log4cplus/configurator.h>
#include <log4cplus/layout.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/consoleappender.h>

Logger::Logger(const std::string &logFilePath)
    : mRootLogger(log4cplus::Logger::getRoot())
{
    log4cplus::PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT(logFilePath));
    printLog("Logger alive!");
}

void Logger::printLog(const std::string &message)
{
    // TODO add to use defferent log levels
    LOG4CPLUS_TRACE(mRootLogger, message << std::endl);
}