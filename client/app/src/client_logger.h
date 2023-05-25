#include <log4cplus/logger.h>

class ClientLogger
{
private:
    log4cplus::Logger logger;

public:
    ClientLogger(const std::string &path);
    void log(const log4cplus::LogLevel &level, const std::string &message);
    bool hasAppenders();
};
