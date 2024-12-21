### objectives: order of implementation

0. xml read
0. encoding - not dumb client application
0. the already defined api
0. conc_serv
0. login logout and register
0. random generator of xml files

### disconnection

- if clients disconnect during operation => server warning
- same for server shutdown => close clients with appropriate message

### command encoding 

- let's send only one byte to server
- UDP commands belongs to [250, 255]
- TCP commands belongs to [5, 10]
- we will avoid 0 and 1 - too common interpretations

| code | command    |
|:----:|:----------:|
| 250  | routes     |
| 251  | departures |
| 252  | arrivals   |
| 5    | report     |
| 6    | quit       |

### random ideas

- all errno numbers: /usr/include/asm-generic/errno.h
- if the server has problems with clients, it won't shut down; instead the clients will be forced to restart the connection
