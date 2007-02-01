/* $Id: global.h,v 1.16 2003/06/06 13:30:19 sjoerd Exp $ */
/* This file must be included by all source files of ffrenzy */
#ifndef GLOBAL_H
#define GLOBAL_H

#ifdef HAVE_STDINT_H
  #include <stdint.h>
#endif

#ifdef ENABLE_NLS
#   include <libintl.h>
#   define _(s) gettext(s)
#   ifdef gettext_noop
#       define N_(String) gettext_noop (String)
#   else
#       define N_(String) (String)
#   endif
#else
#   define N_(String) (String)
#   define _(s) (s)
#endif

#define FALSE 0
#define TRUE  (!FALSE)

#ifndef MAX
  #define MAX(x,y) ((x) > (y) ? (x) : (y))
#endif
#ifndef MIN
  #define MIN(x,y) ((x) < (y) ? (x) : (y))
#endif
#ifndef SGN
  #define SGN(x)   ((x) == 0 ? 0 : ((x) > 0 ? 1 : -1))
#endif

#ifndef NDEBUG
  extern int debugmask;
  #define DEBUG(mask,title,...) do { if (debugmask & mask) { \
                                       fprintf(stderr,"%s: ",title); \
                                       fprintf(stderr,__VA_ARGS__); \
                                       fprintf(stderr,"\n"); \
                                     }    \
                                } while(0) 
  #define DGRAPHICS 0x1
  #define DMODEL    0x2
  #define DNETWORK  0x4
  #define DLOCALIO  0x8
  #define DMAIN     0x10
  #define DCOMM     0x20

#else /* NDEBUG */
  #define DEBUG(mask,title,...)  do {} while (0)
#endif

#define WARN(...) do { fprintf(stderr,__VA_ARGS__); \
                       fprintf(stderr,"\n"); \
                  } while(0)

#define INFO(...) do { printf(__VA_ARGS__); \
                       printf("\n");  \
                  } while(0)
#endif /* GLOBAL_H */
