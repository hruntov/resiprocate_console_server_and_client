# SIPp test scenarios

This document provides a detailed guide on how to create and use a SIPp scenario that test the server.


### Scenario file: message_scenario.xml

The scenario file auth_and_message_to_server.xml is an XML file containing the SIP message scenario that will be used by SIPp. This scenario is designed to send a SIP REGISTER and MESSAGE request to the target server.


### Running the scenario

To run the SIPp scenario using the **message_scenario.xml** file, use the following command:
`sipp -sf test/etc/sipp/auth_and_message_to_server.xml -r 1 -rp 1000 -d 2000 -m 1 -t u1 -p 5080 172.17.0.2:5070`


### Command explanation

- -sf test/etc/message_scenario.xml: Specifies the path to the XML scenario file.
- -r 1: Sets the call rate (calls per second) to 1.
- -rp 1000: Specifies the rate period, in milliseconds, for the call rate (1000 ms = 1 s).
- -d 2000: Sets the duration of the call (in milliseconds) to 2000 ms (2 s).
- -t u1: Specifies the transport type as UDP and sets the socket mode to "one socket per call."
- -p 5080: Sets the local SIP port number to 5080.
- 172.17.0.2:5070: Specifies the target SIP server's IP address and port number


### Scenario file: forward_scenario.xml

The scenario file forward_scenario.xml is an XML file that contains a SIP message scenario used by SIPp. This scenario is designed to forward a SIP MESSAGE request from John to Bob after authentication.


### Running the scenario

To run the SIPp scenario using the forward_scenario.xml file, use the following command:
`sipp -sf test/etc/sipp/forwarding_scenario.xml -r 1 -rp 1000 -d 2000 -m 1 -t u1 -p 5080 172.17.0.2:5070`


### Command Explanation

- -sf forward_scenario.xml: Specifies the path to the XML scenario file.
- -r 1: Sets the call rate (calls per second) to 1.
- -rp 1000: Specifies the rate period, in milliseconds, for the call rate (1000 ms = 1 s).
- -d 2000: Sets the duration of the call (in milliseconds) to 2000 ms (2 s).
- -t u1: Specifies the transport type as UDP and sets the socket mode to "one socket per call."
- -p 5080: Sets the local SIP port number to 5080.
- 172.17.0.2:5070: Specifies the target SIP server's IP address and port number.