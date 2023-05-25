#include <client_logger.h>
#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>

ClientLogger::ClientLogger(const std::string &path)
{
    auto tpath = log4cplus::tstring(path.begin(), path.end());

    log4cplus::initialize();

    log4cplus::PropertyConfigurator config(tpath);
    config.configure();

    logger = log4cplus::Logger::getRoot();
}

void ClientLogger::log(const log4cplus::LogLevel &level, const std::string &message)
{
    logger.log(level, log4cplus::tstring(message.begin(), message.end()));
}

bool ClientLogger::hasAppenders()
{
    return !logger.getAllAppenders().empty();
}
