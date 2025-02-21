# Aplicația căilor ferate române

Puteți găsi versiunea acestui document in engleza [aici](./../README.md).

https://github.com/user-attachments/assets/b17811d2-45a2-4e7a-8d8b-987f20e0e028

## Conectare

Pentru a stabili conexiunea cu serverul meu asigurați-vă că:

- instalați [openvpn3](https://community.openvpn.net/openvpn/wiki/OpenVPN3Linux)
- porniți o sesiune folosind acesta [cheie](./docs/vpnkey.ovpn) (openvpn3 session-start --config docs/vpnkey.ovpn)
- trimiteți un e-mail la petrubraha@gmail.com pentru acreditările solicitate la pasul anterior
- compilați și să rulați aplicația client cu următoarele argumente: „10.100.0.30” și "2970

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
