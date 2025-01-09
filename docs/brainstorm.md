### objectives: order of implementation

0. input valiodation for tcp as well

0. please type better names / identation / scoping
0. conc_serv
0. login logout and register
0. cpp rate

### terminology

- set == encode == encrypt
- get == decode == decrpyt

### disconnection

- client disconnects during operation => server warning
- server shutdowns during operation => close clients

### data encoding

- xml data should be minimal and understandable for developers
    - developers should be able to encode correctly the data
    - it is not that relevant to be as minimal as possible since parsing the file returns char*
- server data should be minimal and its decoding should be understandable for developers
    - this should be as minimal as possible => gurantee speed
- data received by client should be minimal and its decoding should be understandable by anyone
    - still minimal as possible
    - the client application will decrypt the data in the most clever way possible

- itinerary is a singleton
- multiple routes can have the same itinerary
- 820 total possible itineraries
- 1440 total possible times
- status will be encoded in departure location in the following way
- 57600 - 1440 * 41
- (57600, 65536)

- approximate
- change to location * time and add extra field for late
- change to location * server store late - bad : no flag

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

### xml part

- reading from it returns xmlChar * => char* => unsigned char


### random ideas

- all errno numbers: /usr/include/asm-generic/errno.h
- if the server has problems with clients, it won't shut down; instead the clients will be forced to restart the connection
- useful function witch doesn't work: xmlKeepBlanksDefault();
