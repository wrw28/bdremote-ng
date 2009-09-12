#ifndef BD_GLOBALDEFS_H
#define BD_GLOBALDEFS_H

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
#  define BDREMOTE_DBG(_ENABLED, _x) if (_ENABLED) { printf("%s:%d -> %s\n", __FILE__, __LINE__, _x); }
#  define BDREMOTE_ERR(x) printf("%s:%d -> ERROR: %s\n", __FILE__, __LINE__, x)
#else
#  define BDREMOTE_DBG(_ENABLED, _x) if (_ENABLED) { printf("%s\n", _x); }
#  define BDREMOTE_ERR(x) printf("Error: %s\n", x)
#endif

/* Macro used to copy a string to the config. */
#define SETVAL(CK, S) {                         \
      if (CK != NULL)                           \
         {                                      \
            free(CK);                           \
            CK = NULL;                          \
         }                                      \
      CK =(char *)malloc(strlen(S)+1);          \
      strcpy(CK, S);                            \
   }

/* Macro used to destroy string members of the config. */
#define FREEVAL(CK) {   \
      if (CK != NULL)   \
         {              \
            free(CK);   \
            CK = NULL;  \
         }              \
   }

#endif /* BD_GLOBALDEFS_H */

