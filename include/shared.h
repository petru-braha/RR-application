#ifndef _0COMMON0_
#define _0COMMON0_

#define NO_FLAG 0

#define BYTES_COMMAND_MAX 40
#define BYTES_OUTCOME_MAX 100
#define BYTES_PATH_MAX 50

#define LEN_ROUTES 6
#define LEN_DEPARTURES 10
#define LEN_ARRIVALS 8
#define LEN_REPORT 6
#define LEN_QUIT 4

#define UDP_STRING_R "routes"
#define UDP_STRING_D "departures"
#define UDP_STRING_A "arrivals"
#define TCP_STRING_R "report"
#define TCP_STRING_Q "quit"

#define UDP_CODE_R 250 // routes command
#define UDP_CODE_D 251 // departures command
#define UDP_CODE_A 252 // arrivals command
#define TCP_CODE_R 5   // report command
#define TCP_CODE_Q 6   // quit command
#define ERR_CODE -1    // standard error code

#define PTCP_SUCCESS 1
#define RECV_FAIL 0

#endif
