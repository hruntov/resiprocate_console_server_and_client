#include <resip/stack/SipStack.hxx>

/**
   This class was made for sending and receiving instant messages via SIP.
**/
class MessageTransceiver
{
private:
    std::unique_ptr<resip::SipStack> mSipStack;
    std::string mUser;
    std::string mServer;
    int mPort = resip::Symbols::DefaultSipPort;

    /// @brief Send a request and wait for response.
    /// @param request request that will be sent.
    /// @return A response message received after sending.
    std::unique_ptr<resip::SipMessage> getResponse(const resip::SipMessage &request);

    /// @brief Make a pair with response code and reason from StatusLine.
    /// @param statusLine StatusLine header.
    /// @return Pair with response code and reason.
    std::pair<int, std::string> makeStatusPair(const resip::StatusLine &statusLine);

public:
    MessageTransceiver(const std::string &user, const std::string &server);
    MessageTransceiver() = delete;

    /// Copy constructor for the class.
    MessageTransceiver(const MessageTransceiver &) = delete;

    /// Copy assignment for the class.
    MessageTransceiver &operator=(const MessageTransceiver &) = delete;

    /// Move constructor for the class.
    MessageTransceiver(MessageTransceiver &&) = delete;

    /// Move assignment for the class.
    MessageTransceiver &operator=(MessageTransceiver &&) = delete;

    /// Destructor for the class.
    ~MessageTransceiver() = default;

    /// @brief Send message to server
    /// @param receiver user that will receive the message.
    /// @param message plain text.
    /// @return Pair with response code and reason.
    std::pair<int, std::string> sendMessage(const std::string &receiver, const std::string &message);

    /// Get a random port number from the dynamic range (49152..65535).
    static int getRandomDynamicPort();

    /// @brief Send REGISTER message to server to authenticate current user.
    /// @param password hashed user's password.
    /// @return Pair with response code and reason.
    std::pair<int, std::string> authenticate(const std::string &password);

    /// @brief Process incoming message from server.
    /// @return Pair with sender name and message text
    std::pair<std::string, std::string> receiveMessage();
};
