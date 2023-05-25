# SIPp Registration and Message receiving Scenario

Run the scenario:
`sipp -sf /app/test/etc/reg-server.xml`

Run IM client:
`./imclient --mode sending --server "127.0.0.1:5060" --config /app/test/etc/user.json --log /app/test/etc/client-logger.conf --receiver john --message "Hi there"`

# SIPp Registration and Message sending Scenario

Run the scenario:
`sipp -sf /app/test/etc/send-server.xml`

Run IM client:
`./imclient --mode receiving --server "127.0.0.1:5060" --config /app/test/etc/user.json --log /app/test/etc/client-logger.conf`
