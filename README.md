# Romanian railways application

You can find the romanian verion of this document [here](./docs/README_RO.md).

## The communication protocol

0. [id_train, time_departure_estimated, time_arrival_estimated] routes(location_departure, location_arrival)
1. [id_train, time_departure_confirmed, location_arrival] departures(location_departure)
2. [id_train, time_arrival_confirmed, location_departure] arrivals(location_arrival)
3. void quit()

## Short list

- locations types
    - departures
    - arrivals

- times types
    - confirmed departure time
    - estimated departure time
    - confirmed arrival time
    - estimated arrival time

## Decisions

- main() function takes ip address and the port as arguments. it approach simulates real connections between different sides. hard coding the connection of a client for a specific server is more suggestive, but less clever.
- running condition of the server should always be true with an exception: mentenance for administration. this allows the administrator (and only him) to shut down the server.
- random geneeration of routes

## Limitations

## Future improvements

## Bibliography

- I preserved a similar code structure with the examples given in the course.

