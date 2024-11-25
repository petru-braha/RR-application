# Romanian railways

You can find the romanian verion of this document [here](./docs/README_RO.md).

## the communication protocol
- [id_train, time_departure_confirmed, location_arrival] routes(location_departure, location_arrival)
- [id_train, time_departure_confirmed, location_arrival] departures(location_departure)
- [id_train, time_arrival_confirmed, location_departure] arrivals(location_arrival)

## extra

- locations types
    - departures
    - arrivals

- times types
    - confirmed departure time
    - estimated departure time
    - confirmed arrival time
    - estimated arrival time
    