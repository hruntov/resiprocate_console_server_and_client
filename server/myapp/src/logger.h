#pragma once

#include <log4cplus/logger.h>

#include <string>

class Logger {
public:
    /**
     * Constructor for the Logger class. This is invoked when a Logger object is created.
     * It initializes the mRootLogger member with the root logger from the log4cplus library.
     * Also, it configures the logger with a given log file path. The method first configures the logger
     * using the log4cplus library's PropertyConfigurator, and then adds a log message to indicate
     * that the logger is up and running.
     *
     * @param logFilePath The file path for the logger config file.
     */
    Logger(const std::string &logFilePath);

    /**
     * Method to print a log message. This method takes a string message as input and logs it
     * as a TRACE level message using the log4cplus library. The log message is appended with a new line.
     * You can check logs in the logs/logfile.log
     */
    void printLog(const std::string &message);

    /**
     * This method is used to configure the logger with a given log config  file path. The method
     * first configures the logger using the log4cplus library's PropertyConfigurator,
     * and then adds a log message to indicate that the logger is up and running.
     */

private:
    log4cplus::Logger mRootLogger;
};