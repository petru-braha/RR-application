# Romanian railways application

You can find the romanian version of this document [here](./docs/README_RO.md).

https://github.com/user-attachments/assets/b17811d2-45a2-4e7a-8d8b-987f20e0e028

## Connect

To establish connection with my server make sure to:

- install [openvpn3](https://community.openvpn.net/openvpn/wiki/OpenVPN3Linux)
- start a session using this vpn [key](./docs/vpnkey.ovpn) (openvpn3 session-start --config docs/vpnkey.ovpn)
- send a mail at petrubraha@gmail.com for the credentials asked at the previous step
- compile and run the client application with the following arguments: '10.100.0.30' and '2970'

## Main files

- [official documentation](./docs/RR_documentation.pdf)
- [client application](./client.c)
- [server application](./server.c)

## Technologies

- C - socket(), bind(), connect(), pthread_t
- C++ - std::mt1337
- libxml2 - [official page](https://gitlab.gnome.org/GNOME/libxml2)

## Features

### The communication protocol

Please consult some definitions from [here](./docs/brainstorm.md)

- [id_train, time_departure, time_arrival, status] routes(location_departure, location_arrival);
- [id_train, time_departure, location_arrival] departures(location_departure);
- [id_train, time_arrival, location_departure] arrivals(location_arrival);
- bool report(id_train, minutes);
- bool quit();

### Speed
	
- prethreaded execution
- create thread for each client
- i/o multiplexing with non-blocking calls

### Correctness

- client uses tcp to server when sending data
- client uses udp to server when sending queries
- server uses tcp to client when sending data

### Security

- specific procedures for possible errors
- solutions if one party stops responding
- test driven-development

## Limitations

- only 1024 users can be connected at once
- not Windows portable
