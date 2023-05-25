#include "user.h"
#include <json.hpp>
#include <fstream>

User::User(const std::string &name, const std::string &password)
    : mName(name), mPassword(password) {}

std::string User::getName()
{
    return mName;
}

std::string User::getPassword()
{
    return mPassword;
}

User User::getUserFromConfig(const std::string &configPath)
{
    std::ifstream file(configPath);
    auto data = nlohmann::json::parse(file);
    std::string name = data["name"];
    std::string password = data["password"];
    return User(name, password);
}
