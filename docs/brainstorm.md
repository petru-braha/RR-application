### to do

0. portability, pthread_t (windows), without xml (old linux) (libraries are ok on faculty's server)
0. xml lib installer
0. better names/function length
0. conc_serv
0. cpp rate
0. change time after an ip address becomes available again after closing server
0. faculty running server

<br>

0. raport: TDD, security if party crashes
0. raport: timed experiment - see conc_serv
0. raport: limitations (introduction), scenarios in conclusions

### definitions

- locations types
    - departures
    - arrivals

- times types
    - confirmed departure time
    - estimated departure time
    - confirmed arrival time
    - estimated arrival time

- function synopsis: return_type name_function parameter(s)
- estimated times = initial times defined by the generated schedule
    - these are the results of clients' commands
- confirmed times = estimated times +/- delays (depends if the train arrives earlier or later)
    - these are logical results which have to be computed by the users

- set == encode == encrypt
- get == decode == decrpyt

- error message
    - must be interpreted by the developer
    - a user won't get an error, if that would be the case he must read the documentation or address to the developer
    - includes details like scope, line, function name, why it didn't work
    - messages about internal operations
- warning message
    - must be interpreted by the user - client.c
    - must be interpreted by the server administrator - server.c
    - includes just a simple instruction, nothing more
    - messages about invalid input arguments - client.c
    - messages about unexpected not treated events, but not errors - server.c

- the time of the application is the current Romania time.
- an itinerary is from point A to point B and no other points exists between these two.
- client's main() function takes ip address and the port as arguments. it approach simulates real connections between different sides. hard coding the connection of a client for a specific server is more suggestive, but less clever.

### data encoding

- xml data should be minimal and understandable for developers
    - developers should be able to encode correctly the data
    - it is not that relevant to be as minimal as possible since parsing the file returns char*
- server data should be minimal and its decoding should be understandable for developers
    - this should be as minimal as possible => gurantee speed
- data received by client should be minimal and its decoding should be understandable by anyone
    - still minimal as possible
    - the client application will decrypt the data in the most clever way possible

- 820 total possible different routes itineraries
- 1440 total possible different times

- an rr_route has
    - id_train - 2 bytes
    - departure_data - 2 bytes
    - arrival_data - 2 bytes
    - delay_data - 1 bytes
- this design of a rr_route has multiple advantages
    - minimal - sizeof(struct rr_route) == 8 bytes
    - if in the future developement another unsigned char field will be necessary, sizeof won't change
    - O(1) data retriving
- departure_data = departure_time * 41 + departure_location (same for arrival_data)
    - 41 == COUNT_LOCATION
    - retrieving the location is simple: departure_location == departure_data % 41
    - retrieving the time is simple: departure_time == departure_data / 41
    - this design allowed the same amount of information but in a compressed manner
- id_train field may look redundant knowing it is the same with the route's index; however its use is proven in udp_communication()
    - skips having multiple calls of sendto()
    - skips having to search again for the right route by index

### command encoding 

- let's send only one byte to server
- UDP commands belongs to [250, 255]
- TCP commands belongs to [5, 10]
- we will avoid 0 and 1 - too common interpretations

- this design allow us to include more commands in future development

| code | command    |
|:----:|:----------:|
| 250  | routes     |
| 251  | departures |
| 252  | arrivals   |
| 5    | report     |
| 6    | quit       |

### decisions

- printf/printing operations should be covered by call() statements - their failure is a matter of interest for the developer, not for user

- after the server starts the loop no fatal errors should be defined - even if something does not go as expected, the server must continue in serving clients no matter what operation failed; if something too bad will happen the kernel itself will stop the process

- the identation pattern used may be weird but helps in visualising multiple files from this project in a IDE

### random ideas

- all errno numbers: /usr/include/asm-generic/errno.h
- if the server has problems with clients, it won't shut down; instead the clients will be forced to restart the connection
- useful function witch doesn't work: xmlKeepBlanksDefault();

- client disconnects during operation => server warning
- server shutdowns during operation => close clients
