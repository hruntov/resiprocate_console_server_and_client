#include "message_transceiver.h"

#include <resip/stack/Helper.hxx>
#include <rutil/Logger.hxx>
#include <resip/stack/PlainContents.hxx>
#include <resip/stack/Symbols.hxx>

static const int SIPSTACK_TIMEOUT_MS = 50;
static const int RECEIVE_ATTEMPTS_NUMBER = 1200; // 1200 attempts * 50 ms = 1 minute
static const int EXPIRES_TIME = 120; // 120 seconds

MessageTransceiver::MessageTransceiver(const std::string &user, const std::string &server)
{
    mServer = server.substr(0, server.find(":"));
    mPort = std::stoi(server.substr(server.find(":") + 1, server.length()));
    mUser = user;

    resip::Log::setLevel(resip::Log::None);
    mSipStack = std::unique_ptr<resip::SipStack>(new resip::SipStack());
    mSipStack->addTransport(resip::UDP, getRandomDynamicPort());
}

std::pair<int, std::string> MessageTransceiver::sendMessage(const std::string &receiver, const std::string &message)
{
    resip::NameAddr destination;
    resip::NameAddr from;
    from.uri().scheme() = resip::Symbols::DefaultSipScheme;
    from.uri().user() = mUser.c_str();
    from.uri().host() = mServer.c_str();
    from.uri().port() = mPort;

    destination.uri().scheme() = resip::Symbols::DefaultSipScheme;
    destination.uri().user() = receiver.c_str();
    destination.uri().host() = mServer.c_str();
    destination.uri().port() = mPort;

    auto request = std::unique_ptr<resip::SipMessage>(resip::Helper::makeMessage(destination, from));
    auto contents = std::unique_ptr<resip::PlainContents>(new resip::PlainContents(message.c_str()));
    request->setContents(contents.get());
    auto response = getResponse(*request);

    if (response)
    {
        return makeStatusPair(response->header(resip::h_StatusLine));
    }
    return std::make_pair(0, "No response");
}

int MessageTransceiver::getRandomDynamicPort()
{
    const int minValue = 49152;
    const int maxValue = 65535;

    return std::rand() % (maxValue - minValue + 1) + minValue;
}

std::pair<int, std::string> MessageTransceiver::authenticate(const std::string &password)
{
    resip::NameAddr from;
    from.uri().scheme() = resip::Symbols::DefaultSipScheme;
    from.uri().user() = mUser.c_str();
    from.uri().host() = mServer.c_str();
    from.uri().port() = mPort;

    auto request = std::unique_ptr<resip::SipMessage>(resip::Helper::makeRegister(from, from));
    auto response = getResponse(*request);

    if (!response)
    {
        return std::make_pair(0, "No response");
    }

    if (response->header(resip::h_StatusLine).responseCode() != 401 &&
        response->header(resip::h_StatusLine).responseCode() != 407)
    {
        return makeStatusPair(response->header(resip::h_StatusLine));
    }

    resip::Data username = mUser.c_str();
    resip::Data passData = password.c_str();
    resip::Data cnonce;
    unsigned int nc = 0;

    auto request2 = std::unique_ptr<resip::SipMessage>(resip::Helper::makeRegister(from, from));
    resip::Helper::addAuthorization(*request2, *response, username, passData, cnonce, nc);

    request2->header(resip::h_Expires).value() = EXPIRES_TIME;
    request2->header(resip::h_CSeq).sequence()++;

    auto response2 = getResponse(*request2);
    if (response2)
    {
        return makeStatusPair(response2->header(resip::h_StatusLine));
    }
    return std::make_pair(0, "No response");
}

std::pair<int, std::string> MessageTransceiver::makeStatusPair(const resip::StatusLine &statusLine)
{
    return std::make_pair(statusLine.responseCode(), statusLine.reason().c_str());
}

std::unique_ptr<resip::SipMessage> MessageTransceiver::getResponse(const resip::SipMessage &request)
{
    mSipStack->send(request);
    std::unique_ptr<resip::SipMessage> response;
    for (int i = 0; i < RECEIVE_ATTEMPTS_NUMBER; i++)
    {
        mSipStack->process(SIPSTACK_TIMEOUT_MS);

        response = std::unique_ptr<resip::SipMessage>(mSipStack->receive());
        if (response && response->isResponse())
        {
            break;
        }
    }

    return response;
}

std::pair<std::string, std::string> MessageTransceiver::receiveMessage()
{
    std::unique_ptr<resip::SipMessage> request;
    for (int i = 0; i < RECEIVE_ATTEMPTS_NUMBER; i++)
    {
        mSipStack->process(SIPSTACK_TIMEOUT_MS);

        request = std::unique_ptr<resip::SipMessage>(mSipStack->receive());
        if (request && request->isRequest() && request->method() == resip::MethodTypes::MESSAGE)
        {
            std::string messageText;
            if (request->getContents())
            {
                messageText = request->getContents()->getBodyData().c_str();
            }
            std::string sender = request->header(resip::h_From).uri().user().c_str();
            auto response = std::unique_ptr<resip::SipMessage>(resip::Helper::makeResponse(*request, 200));
            mSipStack->send(*response);
            return std::make_pair(sender, messageText);
        }
    }

    return std::make_pair("", "");
}
