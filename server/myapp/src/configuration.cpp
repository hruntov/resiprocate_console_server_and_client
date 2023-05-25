#include "configuration.h"

#include <boost/program_options.hpp>

#include <json.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

static const std::string CFG_OPT_NAME = "config";
static const std::string LOG_OPT_NAME = "log";
static const std::string HELP_OPT_NAME = "help";
static const std::string CONF_EXTENSION_NAME = ".conf";


void Configuration::setPath(const std::string &filePath, const std::string &optionName)
{
    if (optionName == CFG_OPT_NAME)
    {
        mConfigPath = filePath;
    }

    else if (optionName == LOG_OPT_NAME)
    {
        mLoggerPath = filePath;
    }
}

void Configuration::initUsersInfo(const std::string &configFilePath)
{
    std::ifstream inputFile(configFilePath);
    if (!inputFile.is_open())
    {
        std::cout << "Failed to open config file." << std::endl;    // TODO: Use logger here. Need to write logPath and configure logger after the read arguments
        return;
    }

    nlohmann::json j;
    inputFile >> j;

    for (const auto &item : j["users"])
    {
        User u;
        u.mUsername = item["username"];
        u.mPasswordHash = item["password_hash"];
        mUserList.emplace_back(u);
    }
}

std::string Configuration::getUsernamePasswordHash(const std::string &username) const
{
    auto it = std::find_if(mUserList.begin(), mUserList.end(), [&username](const User &user)
    {
        return user.mUsername == username;
    });

    if (it != mUserList.end())
    {
        return it->mPasswordHash;
    }
    else
    {
        return "";
    }
}

const std::string Configuration::getFileExtension(const std::string &filePath)
{
    size_t dotPos = filePath.rfind('.');
    if (dotPos == std::string::npos)
    {
        return "";
    }
    return filePath.substr(dotPos);
}

void Configuration::handleOption(const std::string &optionName, const boost::program_options::variables_map &vm)
{
    if (vm.count(optionName)) {
        std::string filePath = vm[optionName].as<std::string>();
        std::cout << optionName << " File = " << filePath << std::endl;    // TODO: Use logger here. Need to write logPath and configure logger after the read arguments

        if (optionName == CFG_OPT_NAME)
        {
            initUsersInfo(filePath);
            setPath(filePath, optionName);
        }

        else if (optionName == LOG_OPT_NAME)
        {
            std::ifstream inputFile(filePath);
            if (!inputFile.is_open() || getFileExtension(filePath) != CONF_EXTENSION_NAME)
            {
                std::cout << "Failed to open " << optionName << " file." << std::endl;    // TODO: Use logger here. Need to write logPath and configure logger after the read arguments
                return;
            }
            setPath(filePath, optionName);
        }

    }
}

void Configuration::processArguments(int argc, char *argv[])
{
    std::string configFilePath;
    std::string logFilePath;

    boost::program_options::options_description desc("Allowed options");
    desc.add_options()("help,h", "Getting help")("config,c", boost::program_options::value<std::string>(), "Entering your path to config file.")("log,l", boost::program_options::value<std::string>(), "Entering your path to log file.");

    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
    boost::program_options::notify(vm);

    if (vm.size() == 0 || vm.count(HELP_OPT_NAME))
    {
        std::cout << desc << std::endl;    // TODO: Use logger here. Need to write logPath and configure logger after the read arguments
    }
    else
    {
        handleOption(CFG_OPT_NAME, vm);
        handleOption(LOG_OPT_NAME, vm);
        mParametersRight = true;
    }
}