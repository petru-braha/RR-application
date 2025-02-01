# Romanian railways application

You can find the romanian version of this document [here](./docs/README_RO.md).

## Usage

- official documentation is avaible [here](./docs/rr_raport.pdf)
- [client application](./client.c)
- [server application](./server.c)

## Technologies

- C - socket(), bind(), connect(), pthread_t
- C++ - std::mt1337
- libxml2 - [official page](https://gitlab.gnome.org/GNOME/libxml2)

## Features

### The communication protocol

Please consult some definitions from [here](./docs/brainstorm.md)

- [id_train, time_departure, time_arrival, status] routes(location_departure, location_arrival)
- [id_train, time_departure, location_arrival] departures(location_departure)
- [id_train, time_arrival, location_departure] arrivals(location_arrival)
- bool report(id_train, minutes)
- bool quit()

### Speed
	
- prethreaded execution
- create thread for each client
- i/o multiplexing with non-blocking calls

### Security

- specific procedures for possible errors
- solution if one party stops responding
- second end of the server

### Correctness

- client uses tcp to server when sending data
- client uses udp to server when sending queries
- server uses tcp to client when sending data

## Limitations

- only 1024 users can be connected at once
- not Windows portable
