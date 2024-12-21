# Aplicația căilor ferate române

Puteți găsi versiunea acestui document in engleza [aici](./../README.md).

## Fișiere principale

- [aplicație client](./../client.c)
- [aplicație server](./../server.c)
- [documentație oficială](./rr_raport.pdf)

## Protocolul de comunicare

0. [id_train, time_departure_estimated, time_arrival_estimated, status] routes(location_departure, location_arrival)
1. [id_train, time_departure_confirmed, location_arrival] plecări(location_departure)
2. [id_train, time_arrival_confirmed, location_departure] arrivals(location_arrival)
3. void report(id_train, minute)
4. void quit()

- synopsis de functie: return_type nume_funcție parametru(i)
- timpii estimați = timpii inițiali definiți de programul generat
- ore confirmate = ore estimate +/- întârzieri (depinde dacă trenul ajunge mai devreme sau mai târziu)

## Descriere

- tipuri de locații
    - plecări
    - sosiri

- tipuri de timpi
    - ora de plecare confirmată
    - ora estimată de plecare
    - ora de sosire confirmată
    - ora estimată de sosire

- ora aplicației este ora curentă a României.
- un itinerar este de la punctul A la punctul B și nu există alte puncte între aceste două puncte.
- funcția main() a clientului ia ca argumente adresa ip și portul. această abordare simulează conexiuni reale între diferite părți. codarea hard a conexiunii unui client pentru un anumit server este mai sugestivă, dar mai puțin inteligentă.

## Implementare

0. corectitudine

- clientul folosește tcp către server atunci când trimite date
- clientul folosește udp la server atunci când trimite interogări
- serverul folosește tcp către client atunci când trimite date

<br>

1. viteza
	
- executie prethreaded
- creare unui nou thread pentru fiecare client
- multiplexare i/o cu ​​apeluri neblocante

<br>

2. securitate

- proceduri specifice pentru erori posibile
- solutie daca un actor se opreste din a raspunde
- alt end pentru server

## De făcut

- alt computer nu se poate conecta la serverul meu
- ce se întâmplă dacă serverul se blochează?
- ce se întâmplă dacă clientul se blochează?
- nu este portabil pentru windows
- depășiți simultan limitarea celor 1024 de clienți

<br>

- raport: experiment cronometrat - vezi conc_serv
- raport: limitări (introducere), scenarii în concluzii
- creșteți siguranța cu două servere care rulează: unul pe computerul meu, unul pe serverul facultății, fiecare actualizare de pe serverul principal va trece la cel de-al doilea
