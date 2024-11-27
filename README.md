# Romanian railways application

You can find the romanian verion of this document [here](./docs/README_RO.md).

## The communication protocol

0. [id_train, time_departure_estimated, time_arrival_estimated, status] routes(location_departure, location_arrival)
1. [id_train, time_departure_confirmed, location_arrival] departures(location_departure)
2. [id_train, time_arrival_confirmed, location_departure] arrivals(location_arrival)
3. void quit()

- function model: return_type name_function parameter(s)
- estimated times = initial times defined by the generated schedule
- confirmed times = estimated times +/- delays (depends if the train arrives earlier or later)
- status has two fields - each indicates whether it has left or arrived
- a clear distinction is made between the word estimated and confirmed. The former has an information initializing character, and while the server is running for that day estimated emphasizes that the information

## Description

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

- running condition of the server should always be true with an exception: mentenance for administration. this allows the administrator (and only him) to shut down the server.
- random geneeration of routes

## Recap

- the server
    - sends data to clients
    - receives data from xml files
    - receives delays from clients
    - updates data 
        - dalays
        - arrival estimation

- data
    - itineraries
    - departure/arrival status
    - delays
    - arrival estimation

## To do

- concurency strategies:
	- child process for each client
	- thread for each client
	- prethreaded execution
- multiplexing will be achieved only with the select() primitive

- ask the professor about
- make it portable? it is only linux now. 
- signal receive?
- tcp - sends data from server to client
- udp - sends command from client to server

## Limitations

## Future improvements

## Bibliography

- I preserved a similar code structure with the examples given in the course.

