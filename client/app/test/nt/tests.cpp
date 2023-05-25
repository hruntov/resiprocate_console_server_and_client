#include "client_logger.h"
#include "message_transceiver.h"
#include "user.h"
#include <gtest/gtest.h>

TEST(ClientLogger, HasNoAppenders_WhenConfigPathIsEmpty)
{
    auto logger = ClientLogger("");

    auto hasAppenders = logger.hasAppenders();

    ASSERT_FALSE(hasAppenders);
}

TEST(ClientLogger, HasNoAppenders_WhenConfigPathIsWrong)
{
    auto logger = ClientLogger("CMakeLists.txt");

    auto hasAppenders = logger.hasAppenders();

    ASSERT_FALSE(hasAppenders);
}

TEST(ClientLogger, HasAppenders_WhenConfigPathIsCorrect)
{
    auto logger = ClientLogger("/app/test/etc/client-logger.conf");

    auto hasAppenders = logger.hasAppenders();

    ASSERT_TRUE(hasAppenders);
}

TEST(MessageTransceiver, GetRandomDynamicPort_AlwaysReturnsLEMaxValue)
{
    const int maxValue = 65535;

    auto port = MessageTransceiver::getRandomDynamicPort();

    ASSERT_LE(port, maxValue);
}

TEST(MessageTransceiver, GetRandomDynamicPort_AlwaysReturnsGEMinValue)
{
    const int minValue = 49152;

    auto port = MessageTransceiver::getRandomDynamicPort();

    ASSERT_GE(port, minValue);
}

TEST(User, ThrowsException_WhenConfigPathIsEmpty)
{
    std::string path = "";

    ASSERT_ANY_THROW(User::getUserFromConfig(path));
}

TEST(User, ThrowsException_WhenConfigPathIsWrong)
{
    std::string path = "/app/test/etc/client-logger.conf";

    ASSERT_ANY_THROW(User::getUserFromConfig(path));
}

TEST(User, HasCorrectName_WhenConfigPathIsCorrect)
{
    std::string path = "/app/test/etc/user.json";
    std::string userName = "Bob";

    auto user = User::getUserFromConfig(path);

    ASSERT_EQ(user.getName(), userName);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
