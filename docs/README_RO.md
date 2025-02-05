# Aplicația căilor ferate române

Puteți găsi versiunea acestui document in engleza [aici](./../README.md).

https://github.com/user-attachments/assets/b17811d2-45a2-4e7a-8d8b-987f20e0e028

## Fișiere principale

- [documentație oficială](./RR_documentation.pdf)
- [aplicație client](./../client.c)
- [aplicație server](./../server.c)

## Tehnologii

- C - socket(), bind(), connect(), pthread_t
- C++ - std::mt1337
- libxml2 - [pagina oficiala](https://gitlab.gnome.org/GNOME/libxml2)

## Caracteristici

### Protocolul de comunicare

- [id_train, time_departure_estimated, time_arrival_estimated, status] routes(location_departure, location_arrival);
- [id_train, time_departure_confirmed, location_arrival] plecări(location_departure);
- [id_train, time_arrival_confirmed, location_departure] arrivals(location_arrival);
- bool report(id_train, minute);
- bool quit();

### Viteza
	
- executie prethreaded
- creare unui nou thread pentru fiecare client
- multiplexare i/o cu ​​apeluri neblocante

### Corectitudine

- clientul folosește tcp către server atunci când trimite date
- clientul folosește udp la server atunci când trimite interogări
- serverul folosește tcp către client atunci când trimite date

### Securitate

- proceduri specifice pentru erori posibile
- solutii daca un actor se opreste din a raspunde
- test driven-development

## Limitari

- doar 1024 de utilizatori pot fi conectati simultan
- neportabil pe Windows
