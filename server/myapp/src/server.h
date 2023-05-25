#include "logger.h"
#include "configuration.h"

#include <resip/stack/SipStack.hxx>
#include <rutil/ThreadIf.hxx>

#include <unordered_map>
#include <chrono>
#include <future>

// ThreadIf is a common resiprocate interface for creating objects that can run in their own threads
class Server : public resip::ThreadIf
{
public:
    /**
     * Constructor for the Server class
     * Takes a reference to a SipStack object, number of calls, transport type, and a ServerLogger object
     */
    Server(Logger& logger, Configuration& config)
      : mLogger(logger),
        mConfig(config)
    {
        // Create a SipStack object, used for managing SIP communications.
        mStack.reset(new resip::SipStack());

        // Set the transport protocol to be used for SIP communication.
        mTransport = resip::UDP;

        // Add the transport protocol to the SIP stack, on port 5070.
        mStack->addTransport(mTransport, 5070);    // TODO: Port should read from config file
    }

    // This method will contain the logic that the thread should execute
    void thread() override;

private:
    // Unique pointer to the SipStack object which manages SIP communications
    std::unique_ptr<resip::SipStack> mStack;

    // Transport protocol to be used for SIP communication
    resip::TransportType mTransport;

    // ServerLogger object to handle logging
    Logger &mLogger;
    // TODO Improve logger initialization

    // Server Configuration object to handle server's configurations
    Configuration& mConfig;

    // Container for storing registered users' URI
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> registeredUsersTime;

    // Container for stroing username string and pair with ip adress and port of clients
    std::unordered_map<std::string, std::pair<std::string, std::string>> registeredUsers;

    /**
     * Forwards a SIP message to the specified URI.
     *
     * @param toUriData The URI data of the destination where the message should be forwarded.
     * @param toUri The string representation of the destination URI.
     * @param bodyData The body data of the SIP message.
     */
    bool forwardMessage(const std::string& recipientUsername, const resip::Data& bodyData);

    bool handleOkResponse(const std::string& responseCallId);

    struct Message
    {
        std::string recipientUsername;
        resip::Data bodyData;
        std::string callId;
    };

    std::vector<Message> messagesInfo;
};