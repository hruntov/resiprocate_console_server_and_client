#pragma once

#include <boost/program_options.hpp>

#include <string>
#include <vector>
#include <map>


/**
 * The Configuration class is designed to process and manage the configuration settings of an application.
 * It provides interfaces to handle command-line arguments, log configuration, and config file with user information.
 *
 * The class utilizes the Boost's program_options library to define and handle command-line options, which include:
 * - 'help' (or 'h'): displays a help message and list of options.
 * - 'config' (or 'c'): specifies the path to a configuration file.
 * - 'log' (or 'l'): specifies the path to a log file.
 *
 * The class also holds a User structure for storing client information including usernames and password hashes.
 */
class Configuration
{
public:
    /**
     * Default constructor for Configuration class.
     * Initializes an empty Configuration object.
     */
    Configuration() = default;

    /**
     * Processes the command-line arguments provided at the launch of the application.
     *
     * @param argc: The number of command-line arguments.
     * @param argv: The array of command-line arguments.
     */
    void processArguments(int argc, char *argv[]);

    /**
     * Retrieves the path of the log configuration file.
     * @return The path to the log configuration file as a string.
     */
    const std::string getLoggerPath() const noexcept
    {
        return mLoggerPath;
    }

    /**
    Retrieves the password hash associated with the given username.
    @param username The username in for which to retrieve the password hash.
    @return The password hash corresponding to the username. If the username is not found, an empty string is returned.
    */
    std::string getUsernamePasswordHash(const std::string &username) const;

    /**
     * Struct User is designed to store information about clients,
     * including their usernames and password hashes.
     */
    struct User
    {
        std::string mUsername;
        std::string mPasswordHash;
    };

/**
     * This is a getter function that checks if the parameters passed to the program are correct.
     * It returns a boolean value, true if the parameters are correct, and false otherwise.
     */
    bool isParametersRight() const noexcept
    {
        return mParametersRight;
    }

private:
    /**
     * This function handles the specific option based on the optionName.
     *
     * It checks if the provided optionName exists in the variables_map object (vm). If it exists, it retrieves the path to the file associated with this option from the vm,
     * prints out this path, and performs specific actions based on the optionName.
     * If the optionName is "config", it calls the initUsersInfo function to read user information from the configuration file and calls the setPath function to update the mParameters map.
     * If the optionName is "log", it checks if the log file can be opened and if it has the ".conf" extension. If these checks pass, it calls the setPath function to update the mParameters map.
     *
     * @param optionName: The name of the option to handle - can be either "config" or "log".
     * @param vm: The variables_map object that contains the parsed command-line options.
     */
    void handleOption(const std::string &optionName, const boost::program_options::variables_map &vm);

    /**
     * Reads a JSON config file with user's info and stores the info as a User structure into mUserList vector.
     *
     * @param configFilePath: The path to the configuration file with users info.
     */
    void initUsersInfo(const std::string &configFilePath);

    /**
     * This function sets the path of the configuration file with user's info or config file for logger based on the provided optionName.
     *
     * It expects two arguments - the filePath which is the path to the file and the optionName which indicates the type of the file (either "config" or "log").
     * Depending on the optionName, it updates the mParameters map which stores the paths to the configuration file with user's info or config file for logger.
     * If the optionName is "config", it updates the "CONFIG_PATH" key in the mParameters map with the provided filePath.
     * If the optionName is "log", it updates the "LOG_PATH" key in the mParameters map with the provided filePath.
     *
     * @param filePath: The path to the configuration file with user's info or config file for logger.
     * @param optionName: The type of the file - can be either "config" or "log".
     */
    void setPath(const std::string &filePath, const std::string &optionName);

    /**
     * Retrieves the file extension of a given file path.
     *
     * @param filePath: The path to the file.
     * @return The file extension as a string.
     */
    const static std::string getFileExtension(const std::string &filePath);

    std::string mLoggerPath;    // Variable with logger config file
    std::string mConfigPath;    // Variable with config file with user's info

    // Vector with User structures, each representing a client.
    std::vector<User> mUserList;

    /**
     * This is a boolean variable that stores the state of the parameters.
     * It is set to true if the parameters are correct, and to false otherwise.
     */
    bool mParametersRight = false;
};