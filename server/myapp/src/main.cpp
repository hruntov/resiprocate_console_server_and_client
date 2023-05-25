#include "configuration.h"
#include "logger.h"
#include "server.h"

int main(int argc, char *argv[])
{
    Configuration config;

    // Process the command line arguments and set the necessary parameters
    // such as the configuration file path and the log file path.
    config.processArguments(argc, argv);
    if (config.isParametersRight() == true)
    {
        // Configure the logger using the log file path specified in the command line arguments.
        // This sets up the logger to output log messages to the specified file.
        Logger logger(config.getLoggerPath());

        // Create a Server object, passing it the SIP stack, the transport protocol, and the logger obj.
        ::Server server(logger, config);  // TODO: with logger define them globally to avoid extra passing it

        // Run the server in its own thread.
        server.run();

        // Wait for the server thread to finish before continuing.
        server.join();

        return 0;
    }
    else
    {
        // If there was an error with the parameters return 0.
        return -1;
    }
    return 0;
}