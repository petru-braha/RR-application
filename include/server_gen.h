#ifndef _00FILES00_
#ifndef _00GENERATE00_
#define _00GENERATE00_

#include <stdlib.h>

struct rr_route schedule[COUNT_ROUTES_MAX];
unsigned short count_routes = 0;

unsigned short generate(struct rr_route *schedule)
{
  unsigned short index_route = 0;
  set_last(&schedule[index_route]);
  count_routes = index_route;

  return count_routes;
}

#endif
#endif
