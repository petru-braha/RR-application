# Aplicația căilor ferate române

Puteți găsi versiunea acestui document in engleza [aici](./../README.md).

<video controls src="video_ro.mp4" title="Title"></video>

## Fișiere principale

- [aplicație client](./../client.c)
- [aplicație server](./../server.c)
- [documentație oficială](./rr_raport.pdf)

## Technologies

- C - socket(), bind(), connect(), pthread_t
- C++ - std::mt1337
- libxml2 - [pagina oficiala](https://gitlab.gnome.org/GNOME/libxml2)

## Caracteristici

### Protocolul de comunicare

0. [id_train, time_departure_estimated, time_arrival_estimated, status] routes(location_departure, location_arrival)
1. [id_train, time_departure_confirmed, location_arrival] plecări(location_departure)
2. [id_train, time_arrival_confirmed, location_departure] arrivals(location_arrival)
3. bool report(id_train, minute)
4. bool quit()

- synopsis de functie: return_type nume_funcție parametru(i)
- timpii estimați = timpii inițiali definiți de programul generat
- ore confirmate = ore estimate +/- întârzieri (depinde dacă trenul ajunge mai devreme sau mai târziu)

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
- solutie daca un actor se opreste din a raspunde

## Limitari

- doar 1024 de utilizatori pot fi conectati simultan
- neportabil pe Windows
