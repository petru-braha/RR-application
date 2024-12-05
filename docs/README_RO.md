# CFR

Aplicatie ce demonstreaza principiile de executie paralela si multiplexare I/O.

## Protocolul va fi alcatuit din urmatoarele comenzi ale clientului
- [id_train, time_departure_estimated, time_arrival_estimated] routes(location_departure, location_arrival)
- [id_train, time_departure_confirmed, location_arrival] departures(location_departure)
- [id_train, time_arrival_confirmed, location_departure] arrivals(location_arrival)

## Lista scurta

- tipuri de locatii
    - de plecare
    - de oprire

- tipuri de timpi
    - de plecare confirmat
    - de plecare estimat
    - de oprire confirmat
    - de oprire estimat
