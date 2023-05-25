#pragma once
#include <string>

/// This class reads and stores user's credentials.
class User
{
private:
    std::string mName;
    std::string mPassword;

public:
    User(const std::string &name, const std::string &password);
    User() = default;
    ~User() = default;

    /// Getter for Name.
    std::string getName();

    /// Getter for Password.
    std::string getPassword();

    /// @brief Generate a User from data in config file.
    /// @param configPath path to config file.
    static User getUserFromConfig(const std::string &configPath);
};
