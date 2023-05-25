#include "client_logger.h"
#include "message_transceiver.h"
#include "user.h"

#include <boost/program_options.hpp>

#include <iostream>
#include <stdexcept>

namespace po = boost::program_options;

po::options_description desc("Allowed options");
po::variables_map args;
std::string mode;
std::string server;
std::string configPath;
std::string logPath;
const int RESPONCE_200_OK = 200;
std::unique_ptr<MessageTransceiver> transceiver;
std::unique_ptr<ClientLogger> logger;

std::string parseArg(const std::string &argName)
{
    if (args.count(argName))
    {
        return args[argName].as<std::string>();
    }

    throw std::invalid_argument("Argument " + argName + " not found.");
}

bool readArgs(const int &argc, const char *argv[])
{
    desc.add_options()
            ("help", "produce help message")
            ("mode", po::value<std::string>(), "sending or receiving")
            ("server", po::value<std::string>(), "server address")
            ("config", po::value<std::string>(), "path to config file")
            ("log", po::value<std::string>(), "path to log file")
            ("receiver", po::value<std::string>(), "recipient's name")
            ("message", po::value<std::string>(), "message that will be sent")
        ;
    try
    {
        po::store(po::parse_command_line(argc, argv, desc), args);
        po::notify(args);

        if (args.size() == 0 || args.count("help"))
        {
            std::cout << desc << std::endl;
            return false;
        }

        mode = parseArg("mode");
        server = parseArg("server");
        configPath = parseArg("config");
        logPath = parseArg("log");
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        std::cout << desc << std::endl;
        return false;
    }

    return true;
}

void runSendingMode(const std::string &receiver, const std::string &message)
{
    auto responce = transceiver->sendMessage(receiver, message);
    if (responce.first == RESPONCE_200_OK)
    {
        logger->log(log4cplus::INFO_LOG_LEVEL, std::string("Delivered to server - " + receiver + ": " + message));
    }
    else
    {
        auto responceMessage = "Sending failed (" + std::to_string(responce.first) + " "
            + responce.second + ").";
        logger->log(log4cplus::WARN_LOG_LEVEL, responceMessage);
    }
}

void runReceivingMode(const std::string &password)
{
    std::cout << "Waiting for incoming messages..." << std::endl;
    while (true)
    {
        auto message = transceiver->receiveMessage();
        if (message.first != "")
        {
            auto fullMessage = "Received from " + message.first + ": " + message.second;
            logger->log(log4cplus::INFO_LOG_LEVEL, fullMessage);
        }
        else
        {
            transceiver->authenticate(password);
        }
    }
}

int main(const int argc, const char *argv[])
{
    if (!readArgs(argc, argv))
    {
        return -1;
    }

    logger = std::unique_ptr<ClientLogger>(new ClientLogger(logPath));
    if (logger->hasAppenders())
    {
        logger->log(log4cplus::INFO_LOG_LEVEL, "Logging started.");
    }
    else
    {
        std::cout << "Logger failed." << std::endl;
        return -1;
    }

    User user;
    try
    {
        user = User::getUserFromConfig(configPath);
        logger->log(log4cplus::INFO_LOG_LEVEL, "Current user is " + user.getName());
    }
    catch (const std::exception &e)
    {
        logger->log(log4cplus::ERROR_LOG_LEVEL, e.what());
        std::cout << "Reading user's credentials failed." << std::endl;
        return -1;
    }

    try
    {
        transceiver = std::unique_ptr<MessageTransceiver>(new MessageTransceiver(user.getName(), server));
    }
    catch (const std::exception &e)
    {
        std::cout << "Creating transceiver failed." << std::endl;
        logger->log(log4cplus::ERROR_LOG_LEVEL, e.what());
        return -1;
    }

    auto authResult = transceiver->authenticate(user.getPassword());

    if (authResult.first == RESPONCE_200_OK)
    {
        logger->log(log4cplus::INFO_LOG_LEVEL, "Authenticated on server.");
    }
    else
    {
        auto responceMessage = "Authentication failed (" +
            std::to_string(authResult.first) + " " + authResult.second + ").";

        logger->log(log4cplus::WARN_LOG_LEVEL, responceMessage);
        return -1;
    }

    if (mode == "sending")
    {
        try
        {
            std::string receiver = parseArg("receiver");
            std::string message = parseArg("message");
            runSendingMode(receiver, message);
        }
        catch (const std::invalid_argument &e)
        {
            logger->log(log4cplus::ERROR_LOG_LEVEL, e.what());
            std::cout << desc << std::endl;
            return -1;
        }
        catch (const std::exception &e)
        {
            logger->log(log4cplus::ERROR_LOG_LEVEL, std::string("Sending failed. ") + e.what());
        }
    }
    else if (mode == "receiving")
    {
        runReceivingMode(user.getPassword());
    }
    else
    {
        std::cout << "Undefined mode!" << std::endl;
        return -1;
    }

    return 0;
}
