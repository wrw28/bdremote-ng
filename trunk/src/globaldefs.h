#ifndef _GLOBALDEFS_H
#define _GLOBALDEFS_H

#ifndef BDREMOTE_FAIL
#  define BDREMOTE_FAIL (-1)
#endif

#ifndef BDREMOTE_OK
#  define BDREMOTE_OK (0)
#endif

#ifndef BDREMOTE_DEBUG
#  define BDREMOTE_DEBUG 0
#endif

#if BDREMOTE_DEBUG
#  include <stdio.h>
#  define BDREMOTE_DBG(x) printf("%s:%d -> %s\n", __FILE__, __LINE__, x)
#  define BDREMOTE_ERR(x) printf("%s:%d -> ERROR: %s\n", __FILE__, __LINE__, x)
#else
#  define BDREMOTE_DBG(x)
#  define BDREMOTE_ERR(x) printf("Error: %s\n", x)
#endif


#endif // _GLOBALDEFS_H

