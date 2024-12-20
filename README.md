# Romanian railways application

You can find the romanian version of this document [here](./docs/README_RO.md).

## Main files

- [client application](./client.c)
- [server application](./server.c)
- [official documentation](./docs/rr_raport.pdf)

## The communication protocol

0. [id_train, time_departure_estimated, time_arrival_estimated, status] routes(location_departure, location_arrival)
1. [id_train, time_departure_confirmed, location_arrival] departures(location_departure)
2. [id_train, time_arrival_confirmed, location_departure] arrivals(location_arrival)
3. void report(id_train, minutes)
4. void quit()

- function synopsis: return_type name_function parameter(s)
- estimated times = initial times defined by the generated schedule
- confirmed times = estimated times +/- delays (depends if the train arrives earlier or later)

## Description

- locations types
    - departures
    - arrivals

- times types
    - confirmed departure time
    - estimated departure time
    - confirmed arrival time
    - estimated arrival time

- the time of the application is the current Romania time.
- an itinerary is from point A to point B and no other points exists between these two.
- client's main() function takes ip address and the port as arguments. it approach simulates real connections between different sides. hard coding the connection of a client for a specific server is more suggestive, but less clever.

## Implementation

0. correctness

- client uses tcp to server when sending data
- client uses udp to server when sending queries
- server uses tcp to client when sending data

<br>

1. speed
	
- prethreaded execution
- create thread for each client
- i/o multiplexing with non-blocking calls

## To do

- testing tcp and udp build: what if server crashes? what if client crashes?
- not portable for windows
- command encoding => 1 byte transmission VS
- overcome the 1024 clients at once limitation

<br>

- raport: timed experiment - see conc_serv
- raport: limitations (introduction), scenarios in conclusions
- increase safety with two running servers: one on my computer, one on the faculty's server, every update on the main server will tcp its way to the second one
