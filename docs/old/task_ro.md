Guidelines for Developing Homework 2

The homework will include two deliverables: a technical report and an implementation.


Technical Report

The technical report must be prepared according to the LNCS format, available on the Springer LNCS Guidelines web page (https://www.springer.com/gp/computer-science/lncs/conference-proceedings-guidelines). The report will follow a structure divided into sections, as follows:

1. Introduction: Present the overall vision and objectives of the project.
2. Applied Technologies: Detail the specific technologies used (for example, TCP, UDP, etc.), with an emphasis on aspects relevant to the project. Explain the motivations behind the selection of these technologies.
3. Application Structure: Expose the concepts used in modeling and present a detailed diagram of the application.
4. Implementation Aspects: Present specific and innovative sections of the project code, providing adequate documentation; present the application-level protocol you will implement; describe real usage scenarios.
5. Conclusions: Analyze potential improvements of the proposed solution.
6. Bibliographic References

Notes:
- sections 1 and 2 will have a maximum size of one page (cumulatively);
- for creating the detailed diagram of the application, you can also use tools such as: https://sequencediagram.org


Implementation

The implementation will consist of developing a server application and a client application.

The server will have the following characteristics/functionalities (according to the details stipulated in the technical report):
- implementation of the concurrency;
- implementation of the communication protocol; at least the part of recognizing commands (and parameters, if applicable) will be implemented, without the actual functionalities; the server will return to the client a confirmation message for the identification of each specific command.

The client will have the following characteristics/functionalities (according to the details stipulated in the technical report):
- implementation of the communication protocol;
- communication with the server (transmission of commands to it and reception of related messages);
- in the case of projects consisting only of a client with a graphical interface (explicitly stated in the description), a prototype must be developed that includes a part of the graphical interface, which demonstrates communication with the server for all functionalities to be implemented.

Note:
- the code must compile and be runnable.


The deadline for submission is the laboratory in week 11. A creative and personal approach is expected in the development of the homework, to ensure the uniqueness and originality of your project.


Project

Sa se implementeze un server ce ofera sau actualizeaza informatii in timp real de la toti clientii inregistrati pentru: mersul trenurilor, status plecari, status sosiri, intarzieri si estimare sosire. Serverul citeste datele din fisiere xml si actualizeaza datele(intarzieri si estimare sosire) la cererea clientilor (signal - receive) Toata logica va fi realizata in server, clientul doar cere informatii despre plecari/sosiri si trimite informatii la server despre posible intarzieri si estimare sosire.

## Activitati
- trimitere de la server a informatiilor despre mersul trenurilor in ziua respectiva
- trimitere de la server a informatiilor despre plecari in urmatoarea ora (conform cu planul, in intarziere cu x min) doar la cererea unui client
- trimitere de la server a informatiilor despre sosiri in urmatoarea ora (conform cu planul, in intarziere cu x min, cu x min mai devreme) doar la cererea unui client
