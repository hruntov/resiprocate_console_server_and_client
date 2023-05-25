#include "server.h"
#include "configuration.h"
#include "logger.h"

#include <resip/stack/Helper.hxx>
#include <resip/stack/SipMessage.hxx>
#include <resip/stack/Uri.hxx>
#include <resip/stack/SipStack.hxx>
#include <rutil/ThreadIf.hxx>
#include <resip/stack/PlainContents.hxx>

#include <chrono>


static const char* sipScheme  = "Digest";
static const char* sipUser = "server";
const resip::Data realm = "server.com";
static const int waitMessageTime = 1000;
static const std::string serverIp = "172.17.0.2";   // TODO: It should read from config file
static const int serverPort = 5070;    // TODO: It should read from config file
static const int code200 = 200;
static const std::string log200 = "200 OK Sent";


void Server::thread()
{
    std::ostringstream logStream;
    logStream << "Server started with socket " << resip::Tuple::toData(mTransport) << ":" << serverIp << ":" << serverPort;
    mLogger.printLog(logStream.str());  // TODO: Seems such approach requires extra method for logger void printLog(std::ostringstream &ostr);

    resip::NameAddr contact;
    contact.uri().scheme() = sipScheme;
    contact.uri().user() = sipUser;
    contact.uri().host() = resip::SipStack::getHostname();
    contact.uri().port() = serverPort;
    /**
     * The transport parameter is set in the URI to indicate the transport protocol used (UDP, TCP, TLS, etc).
     * It is converted to a string using the toData() function.
     */
    contact.uri().param(resip::p_transport) = resip::Tuple::toData(mTransport);

    while (!isShutdown())
    {
        mStack->process(waitMessageTime);
        std::unique_ptr<resip::SipMessage> received(mStack->receive());
        if (received)
        {
            const resip::NameAddr fromField = received->header(resip::h_From);
            const std::string senderUsername = fromField.uri().user().c_str();
            resip::Data uriData = fromField.uri().toString();
            std::string uri(uriData.data(), uriData.size());
            mLogger.printLog("Received something from user: " + senderUsername);

            // if (received->isResponse() && received->header(resip::h_StatusLine).responseCode() == 200)
            // {
            //     mLogger.printLog("Received 200 OK response.");
            //     const resip::Data responseCallIdData = received->header(resip::h_CallID).value();
            //     const std::string responseCallIdStr = std::string(responseCallIdData.data(), responseCallIdData.size());
            //     // handleOkResponse(responseCallIdStr);

            // }

            // Check registration
            if (registeredUsersTime.find(uri) == registeredUsersTime.end())    // No registration
            {
                const std::string passwordHash = mConfig.getUsernamePasswordHash(senderUsername);
                const resip::Data passwordHashData(passwordHash);

                const resip::Helper::AuthResult result = resip::Helper::authenticateRequest(*received, realm, passwordHashData);

                if (result == resip::Helper::Authenticated)
                {
                    std::unique_ptr<resip::SipMessage> msg200(resip::Helper::makeResponse(*received, code200, contact));
                    mStack->send(*msg200);
                    mLogger.printLog(log200);

                    const unsigned int expiresValue = received->header(resip::h_Expires).value();
                    registeredUsersTime[uri] = std::chrono::steady_clock::now() + std::chrono::minutes(expiresValue);   // TODO: Add removing users after expired time end

                    const resip::Via &viaHeader = received->header(resip::h_Vias).front();
                    resip::Data viaHostData = viaHeader.sentHost();
                    std::string viaHost(viaHostData.data(), viaHostData.size());
                    std::string viaPort = std::to_string(viaHeader.sentPort());
                    std::pair<std::string, std::string> viaHostAndPort = std::make_pair(viaHost, viaPort);
                    registeredUsers[senderUsername] = viaHostAndPort;
                }

                else
                {
                    std::unique_ptr<resip::SipMessage> msg401(resip::Helper::makeResponse(*received, 401, contact));

                    resip::Auth authChallenge;
                    authChallenge.scheme() = sipScheme;
                    authChallenge.param(resip::p_nonce) = resip::Helper::makeNonce(*received, resip::Data(resip::Timer::getTimeSecs()));

                    authChallenge.param(resip::p_realm) = realm;

                    msg401->header(resip::h_WWWAuthenticates).push_front(authChallenge);
                    mStack->send(*msg401);
                    mLogger.printLog("Checking loggining info...(401 sent)");
                }
            }

            else    // Have registration
            {
                mLogger.printLog("User: " + senderUsername + " is registered.");
                const resip::MethodTypes meth = received->header(resip::h_RequestLine).getMethod();
                if (meth == resip::REGISTER)
                {
                    std::unique_ptr<resip::SipMessage> msg200(resip::Helper::makeResponse(*received, code200, contact));
                    mStack->send(*msg200);
                    mLogger.printLog(log200);
                }

                else if (meth == resip::MESSAGE)
                {
                    std::unique_ptr<resip::SipMessage> msg200(resip::Helper::makeResponse(*received, code200, contact));
                    const resip::Contents *contents = received->getContents();

                    if (contents)
                    {
                        resip::Data bodyData(contents->getBodyData());
                        const resip::NameAddr toField = received->header(resip::h_To);
                        resip::Data toUriData = toField.uri().toString();
                        std::string recipientUri(toUriData.data(), toUriData.size());

                        // Extract the recipient username from the URI
                        std::string recipientUsername;
                        size_t colonPos = recipientUri.find('@');
                        if (colonPos != std::string::npos)
                        {
                            recipientUsername = recipientUri.substr(4, colonPos - 4);
                        }

                        else
                        {
                            mLogger.printLog("NO RECIPITENT!");
                        }

                        // Check if 'To' user is registered
                        if (registeredUsersTime.find(recipientUri) != registeredUsersTime.end()) // User is registered
                        {
                            if (forwardMessage(recipientUsername, bodyData))
                            {
                                const resip::NameAddr fromField = received->header(resip::h_From);
                                resip::Data fromFieldData = fromField.uri().toString();
                                const std::string fromStr = std::string(fromFieldData.data(), fromFieldData.size());

                                resip::Data callIdData = received->header(resip::h_CallID).value();
                                const std::string callIdStr = std::string(callIdData.data(), callIdData.size());

                                mLogger.printLog("Forwarded message from: " + senderUsername + " to: " + recipientUsername + " Call-ID: " + callIdStr);

                                mStack->send(*msg200);
                                mLogger.printLog(log200);
                            }
                        }

                        else
                        {
                            mLogger.printLog("User " + recipientUsername + " is not registered. Cannot forward message.");
                        }
                    }

                    else
                    {
                    mLogger.printLog("Message has no body");
                    }
                }
            }
        }
    }
}


bool Server::forwardMessage(const std::string& recipientUsername, const resip::Data& bodyData)
{
    auto it = registeredUsers.find(recipientUsername);
    if (it != registeredUsers.end())
    {
        resip::NameAddr to = resip::NameAddr();
        std::string recipientIp = it->second.first;
        std::string recipientPort = it->second.second;

        to.uri().param(resip::p_transport) = resip::Tuple::toData(mTransport);
        to.uri().scheme() = sipScheme;
        to.uri().user() = recipientUsername.c_str();
        to.uri().host() = resip::Data(recipientIp.c_str());
        to.uri().port() = std::stoi(recipientPort);

        resip::NameAddr from = resip::NameAddr();
        from.uri().scheme() = sipScheme;
        from.uri().user() = sipUser;
        from.uri().host() = resip::SipStack::getHostname();
        from.uri().port() = serverPort;
        from.uri().param(resip::p_transport) = resip::Tuple::toData(mTransport);

        resip::SipMessage* rawMessage = resip::Helper::makeMessage(to, from);
        std::unique_ptr<resip::SipMessage> message(rawMessage);

        const std::shared_ptr<resip::Contents> contents = std::make_shared<resip::PlainContents>(bodyData);
        message->setContents(contents.get());
        message->header(resip::h_ContentType) = resip::Mime("text", "plain");

        resip::H_ContentLength::Type contentLengthValue;
        contentLengthValue.value() = bodyData.size();
        message->header(resip::h_ContentLength) = contentLengthValue;

        try
        {
        mStack->send(*message);
        mLogger.printLog("Message sent to: " + recipientUsername);
        return true;
        }

        catch (const std::exception& e)
        {
            mLogger.printLog("Message failed to send. Error: " + std::string(e.what()));
            return false;
        }
    }
    else
    {
        return false;
    }
}

bool Server::handleOkResponse(const std::string& responseCallId)
{
    for (auto it = messagesInfo.begin(); it != messagesInfo.end(); ++it)
    {
        if (it->callId == responseCallId)
        {
            messagesInfo.erase(it);
            return true;
        }
    }
    return false;
}