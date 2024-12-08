# Romanian railways application

You can find the romanian verion of this document [here](./docs/README_RO.md).

## The communication protocol

0. [id_train, time_departure_estimated, time_arrival_estimated, status] routes(location_departure, location_arrival)
1. [id_train, time_departure_confirmed, location_arrival] departures(location_departure)
2. [id_train, time_arrival_confirmed, location_departure] arrivals(location_arrival)
4. void quit()

- function model: return_type name_function parameter(s)
- estimated times = initial times defined by the generated schedule
- confirmed times = estimated times +/- delays (depends if the train arrives earlier or later)
- status has two fields - each indicates whether it has left or arrived
- a clear distinction is made between the word estimated and confirmed. The former has an information initializing character, and while the server is running for that day estimated emphasizes that the information

## Description

- the time of the application is the current Romania time.
- an itinerary is from point A to point B and no other points exists between these two. If that would be the case then extra complexity would be added to the logic of the application.

- locations types
    - departures
    - arrivals

- times types
    - confirmed departure time
    - estimated departure time
    - confirmed arrival time
    - estimated arrival time

- multiplexing
- tcp
- udp

- client's main() function takes ip address and the port as arguments. it approach simulates real connections between different sides. hard coding the connection of a client for a specific server is more suggestive, but less clever.
- departures/arrivals status = bool returned by a function that takes an id_train - if it has left or not (analog arrived). 
- the activities described seem to be a service used by customers who want to travel, not who are traveling. It seems strange to me that a non-traveling customer would give information about delays. How would they know? I expect the customer to be so, on a train or not. I suppose only if he is on the train can he send that information. The delay can be influenced by a late departure or the train staff announcement (true announcement all the time). Therefore, a client will be queried by the server at first if it is on a train. If yes, then it will send the minutes by which it will be late (in case of a delay announcement; arrival estimate is inferred from departure estimate + announced delays). 

- what if two clients reports different things? they can report for past, present and futures trains. the previous statement limited the scope of this question to the present trains. (reporting problem)

- running condition of the server should always be true with an exception: mentenance for administration. this allows the administrator (and only him) to shut down the server.
- random geneeration of routes

- ip addresses
    - 127.0.0.1 locally
    - one connection to my server of the faculty
    - one more connection when my pc is up
- port = 1010

- observation! the file descriptors will never be ambigous! the client application doesn't allow it!

## Recap

- the server
    - sends data to clients
    - receives data from xml files
    - receives delays from clients
    - updates data 
        - dalays
        - arrival estimation

- data
    - routes
    - departure/arrival status
    - delays
    - arrival estimation

## Implementation

0. correctness
1. speed
2. array from port to sd

- suppose 100.000 users will connect to my server now.

0. tcp over udp: if the data sent by the client is altered => wrong number of minutes => the schedule becomes incorrect. it's okay if the client's query is not received from the first attempt, but the data received by him has to be not affected and accurate.

- decision:
a. client uses tcp to server when sending data
b. client uses udp to server when sending queries
c. server uses tcp to client when sending data

1. concurency strategies:
	- preforked execution
	- prethreaded execution
	- create child process for each client
	- create thread for each client
	- i/o multiplexing with blocking calls
	- i/o multiplexing with non-blocking calls

- using select() and unblocking I/O operations is the fastest strategy. how can i make it faster? (fastest <- course)

- thread over child process
	- smaller cost (course example of 50.000)
	- there can be a lot of writes on the server data
	- the child process will copy by ref but threads plays with the parent process' variables themselves

- what to multiplex? in a non-concurent server multiplexing solves all their quieries. but since i'm threading the environement... it will still do the same thing. There will be a global I/O multiplexing, each thread accessing it.

a. the sever uses i/o multiplexing and serves the ready file descriptors concurently using threads

- what about prethreaded VS thread per client?

- ask the professor about: signal - receive
- final implementation idea: server prethreaded accept calls, a thread is added for any other client. i/o multiplexing

- send back only tcp? array of descriptors?
- what if server crashes?
- what if client crashes?
- sizeof(struct sockaddr) or sizeof(skadd_server) ?
- POSIX standard allows only 1024 => 1024 * 98 > 100.000

## To do / questions / future improvements

- experiment

- increase safety with two running servers: one on my computer, one on the faculty's server, every update on the main server will tcp its way to the second one

- random generator std::hash<> bug
- not portable for windows
- 1024 clients at once

## Notes

- for each client set up sd_tcp and sd_udp
- it is not a bug: the value of the first file descriptor initialized can varry
- to explain more here

## Bibliography

- I preserved a similar code structure with the examples given in the course.

