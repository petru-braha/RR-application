- cool path to explore: /usr/include/asm-generic/errno.h

- if the clients suddenly disconnect => server warning
- if the server process is stoped/crashed => close clients with appropriate message 

- if the server has problems with clients, it won't be shut down, instead the clients will be forced to restart the connection

- for the tcp developement: /tests/random/read_all => /tests/ => /